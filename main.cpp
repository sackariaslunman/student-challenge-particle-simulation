#include <iostream>
#include <vector>
#include <array>
#include <math.h>
#include <algorithm>
#include <string>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <functional>

using namespace std;

using point = array<double, 3>;
string to_string(const point & p) {
  return to_string(p[0]) + " " + to_string(p[1]) + " " + to_string(p[2]);
}

using ppair = array<point, 2>;
string to_string(const ppair & pair) {
  return "(" + to_string(pair[0]) + "), (" + to_string(pair[1]) + ")";
}

template<typename T, typename F>
int binary_search_(const vector<T> & array, int lo, int hi, const F & key, bool last=false) {
  int result = -1;
  int mid;
  double comparator;

  while (lo < hi) {
    mid = (hi + lo) / 2;
    comparator = key(array[mid]);

    if (comparator == 0) {
      result = mid;
      if (last) {
        lo = mid + 1;
      }
      else {
        hi = mid;
      }
    }
    else if (comparator < 0) {
      lo = mid + 1;
    }
    else {
      hi = mid;
    }
  }
  return result;
}


double distance(const point & p1, const point & p2) {
  double d = 0;
  int i;
  for (i = 0; i < p1.size(); i++) {
    d += pow(p1[i] - p2[i], 2);
  }
  return sqrt(d);
};

template <typename T>
bool includes(const vector<T> & vec, const T & elem) {
  if(find(vec.begin(), vec.end(), elem) != vec.end()) {
    return true;
  }
  return false;
}

void brute_force(const vector<point> & points, int lo, int hi, double min_d, vector<ppair> & pairs, int lo1, int hi1) {
  vector<ppair> local_pairs;
  int i;
  int j;
  point p1;
  point p2;
  double d;

  for (i = lo; i < hi; i++) {
    for (j = lo1; j < hi1; j++) {
      p1 = points[i];
      p2 = points[j];
      d = distance(p1, p2);
      if (d < min_d && !includes(pairs, {p1, p2}) && !includes(pairs, {p2, p1})) {
        pairs.push_back({p1, p2});
      }
    }
  }
  pairs.insert(pairs.end(), local_pairs.begin(), local_pairs.end());
}

void brute_force(const vector<point> & points, int lo, int hi, double min_d, vector<ppair> & pairs) {
  brute_force(points, lo, hi, min_d, pairs, lo, hi);
};

double close_to_mid(const point & p, const point & mid_point, double min_d, int dim) {
  auto d = p[dim] - mid_point[dim];
  if (abs(d) <= min_d) {
    return 0;
  }
  return d;
}

void find_close_pairs_recursion(vector<point> & points, int lo, int hi, double min_d, int cutoff, int dim_depth, const vector<string> & verbose, int dim, vector<ppair> & pairs, int & calls) {
  auto size = hi - lo;
  calls++;
  if (size <= cutoff) {
    brute_force(points, lo, hi, min_d, pairs);
    return;
  }

  auto mid = (lo + hi) / 2;
  auto mid_point = points[mid];

  auto lo_close_to_mid = close_to_mid(points[lo], mid_point, min_d, dim) == 0;
  auto hi_close_to_mid = close_to_mid(points[hi-1], mid_point, min_d, dim) == 0;

  if ((lo_close_to_mid || hi_close_to_mid) && (dim_depth - dim > 1)) {
    // vector<point> local_points(points.begin()+lo, points.begin()+hi);
    sort(points.begin()+lo, points.begin()+hi, [dim](const point & p1, const point & p2) -> bool {
      return p1[dim + 1] < p2[dim + 1];
    });
    if (includes(verbose, string("dim"))) {
      cout << "next dim! dim " << dim << " to dim " << dim+1 << "\n";
    }
    find_close_pairs_recursion(points, lo, hi, min_d, cutoff, dim_depth, verbose, dim+1, pairs, calls);
    return;
  }

  // auto mid_lo1 = mid - 1;
  // while (mid_lo1 > lo && close_to_mid(points[mid_lo1], mid_point, min_d, dim) == 0) {
  //   mid_lo1--;
  // }

  auto mid_lo = lo;
  if (!lo_close_to_mid) {
    mid_lo = binary_search_(
      points,
      lo, 
      hi, 
      [dim, mid_point, min_d](const point & p) -> double {
        return close_to_mid(p, mid_point, min_d, dim);
      },
      false
    );
  } else if (includes(verbose, string("close"))) {
    cout << "too close lo " << mid-lo << " dim " << dim << "\n";
  }

  // auto mid_hi1 = mid + 1;
  // while (mid_hi1 < hi && close_to_mid(points[mid_hi1], mid_point, min_d, dim) == 0) {
  //   mid_hi1++;
  // }

  auto mid_hi = hi;
  if (!hi_close_to_mid) {
    mid_hi = binary_search_(
      points, 
      mid, 
      hi, 
      [dim, mid_point, min_d](const point & p) -> double {
        return close_to_mid(p, mid_point, min_d, dim);
      },
      true
    ) + 1;
  } else if (includes(verbose, string("close"))) {
    cout << "too close hi " << hi-mid << " dim " << dim << "\n";
  }

  // if (mid_lo != mid_lo1) {
  //   cout << "Difference mid lo! " << mid_lo << " " << mid_lo1 << " " << lo << "\n";
  // }
  // if (mid_hi != mid_hi1) {
  //   cout << "Difference mid hi! " << mid_hi << " " << mid_hi1 << " " << hi << "\n";
  // }

  brute_force(points, mid_lo, mid, min_d, pairs, mid, mid_hi);
  find_close_pairs_recursion(points, lo, mid, min_d, cutoff, dim_depth, verbose, dim, pairs, calls);
  find_close_pairs_recursion(points, mid, hi, min_d, cutoff, dim_depth, verbose, dim, pairs, calls);
}

void find_close_pairs(vector<point> & points, vector<ppair> & pairs, int lo, int hi, double min_d, int cutoff, int dim_depth, int & calls, const vector<string> & verbose={}) {
  find_close_pairs_recursion(points, lo, hi, min_d, cutoff, dim_depth, verbose, 0, pairs, calls);
}

vector<point> read_positions_file(string file_path) {
  vector<point> positions;
  ifstream positions_file;
  string new_text;
  int count = 0;
  
  positions_file.open("data/positions.xyz");

  if (positions_file.is_open()) {
    while (positions_file.good()) {
      point p;
      int i;
      for (i = 0; i < 3; i++) {
        positions_file >> new_text; 
        p[i] = stod(new_text);
      }
      positions.push_back(p);
    }
  }

  return positions;
}

int main(int argc, char** argv) {
  auto positions = read_positions_file("data/positions.xyz");
  cout << "Positions: " << positions.size() << endl;

  double min_d = 0.05;
  if (argc >= 2) {
    min_d = stod(argv[1]);
  }

  int cutoff = 20;
  if (argc >= 3) {
    cutoff = stoi(argv[2]);
  }
  cout << "Parameters: min_d = " << min_d << ", cutoff = " << cutoff << "\n\n";

  for (auto dim_depth : {1,2,3}) {
    int calls = 0;
    cout << "Dim depth = " << dim_depth << "\n";

    auto start_time = chrono::system_clock::now();
    sort(positions.begin(), positions.end(), [](const point & p1, const point & p2) -> int {
      return p1[0] < p2[0];
    });

    cout << "Finding pairs..." << "\n";
    vector<ppair> pairs;
    find_close_pairs(positions, pairs, 0, positions.size(), min_d, cutoff, dim_depth, calls, {""});

    auto end_time = chrono::system_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
    cout << "Finished finding pairs in " << duration << " ms" << "\n";
    cout << "Number of pairs: " << pairs.size() << "\n";
    cout << "Recursion calls i: " << calls << "\n";
    calls = 0;
    cout << "\n";
  }

  return 0;
}