import pandas as pd
import numpy as np


# def generate_points(size, min, max):
#     points = np.random.normal(min, max, (size, 3))
#     return np.ndarray.tolist(points)

# positions = generate_points(100000, -0.5, 0.5)
# positions[:10]

def binary_search(array, lo, hi, key, last=False):
    result = -1
    while lo < hi:
        mid = (hi + lo) // 2
        comparator = key(array[mid])

        if comparator == 0:
            result = mid
            if last:
                lo = mid + 1
            else:
                hi = mid
        elif comparator < 0:
            lo = mid + 1
        else:
            hi = mid
    
    return result

def distance(p1, p2):
    d = 0
    for i in range(len(p1)):
        d += (p1[i] - p2[i])**2
    return d**0.5

def brute_force(points, lo, hi, min_d):
    pairs = []
    for i in range(lo, hi):
        for j in range(lo, hi):
            p1 = points[i]
            p2 = points[j]
            d = distance(p1, p2)
            if d < min_d and (p1, p2) not in pairs and (p2, p1) not in pairs:
                pairs.append((p1, p2))
    return pairs

def find_close_pairs(points, lo, hi, min_d, cutoff, dim_depth, verbose=[]):
    return find_close_pairs_recursion(points, lo, hi, min_d, cutoff, dim_depth, verbose, 0)

def close_to_mid(p, mid_point, min_d, dim):
    d = p[dim] - mid_point[dim]
    if abs(d) <= min_d:
        return 0
    return d

def find_close_pairs_recursion(points, lo, hi, min_d, cutoff, dim_depth, verbose, dim):
    size = hi - lo
    if size < cutoff:
        return brute_force(points, lo, hi, min_d)

    mid = (lo + hi) // 2
    mid_point = points[mid]

    lo_close_to_mid = close_to_mid(points[lo], mid_point, min_d, dim) == 0
    hi_close_to_mid = close_to_mid(points[hi-1], mid_point, min_d, dim) == 0

    if (lo_close_to_mid or hi_close_to_mid) and dim_depth - dim > 1:
        local_lo = 0
        local_hi = len(points[lo:hi])
        local_points = sorted(points[lo:hi], key=lambda p: p[dim + 1])
        if "dim" in verbose:
            print("next dim!", "dim", dim, "to", "dim", dim+1)
        return find_close_pairs_recursion(local_points, local_lo, local_hi, min_d, cutoff, dim_depth, verbose, dim+1)
        
    pairs_lo = find_close_pairs_recursion(points, lo, mid, min_d, cutoff, dim_depth, verbose, dim)
    pairs_hi = find_close_pairs_recursion(points, mid, hi, min_d, cutoff, dim_depth, verbose, dim)

    mid_lo = lo
    if not lo_close_to_mid:
        mid_lo = binary_search(points, lo, mid, lambda p: close_to_mid(p, mid_point, min_d, dim), False)
    elif "close" in verbose:
        print("too close lo", hi-mid, "dim", dim)
        
    mid_hi = hi
    if not hi_close_to_mid:
        mid_hi = binary_search(points, mid, hi, lambda p: close_to_mid(p, mid_point, min_d, dim), True) + 1
    elif "close" in verbose:
        print("too close hi", hi-mid, "dim", dim)

    pairs_mid = []
        
    for i in range(mid_lo, mid):
        for j in range(mid, mid_hi):
            p1 = points[i]
            p2 = points[j]
            d = distance(p1, p2)
            if d < min_d and (p1, p2) not in pairs_mid and (p2, p1) not in pairs_mid:
                pairs_mid.append((p1, p2))

    return pairs_lo + pairs_mid + pairs_hi

import time

def main():
    df_points = pd.read_table("data/positions.xyz", skiprows=2, delim_whitespace=True, names=['x', 'y', 'z'])
    print(df_points)
    positions = np.ndarray.tolist(df_points.to_numpy().reshape(-1, 3))

    positions = sorted(positions, key=lambda p: p[0])
    size = len(positions)//1
    min_d = 0.05

    print(f"Points: {size}")
    for dim_depth in [1,2,3]:
        start_time = time.time()
        q_pairs = find_close_pairs(positions, 0, size, 0.05, 20, dim_depth, verbose=[""])
        total_time = time.time() - start_time
        print(f"Finding pairs with dim_depth = {dim_depth}: {round(total_time, 4)}s. Pairs: {len(q_pairs)}")

    bf_pairs = brute_force(positions, 0, size, 0.05)

    print("Brute force:", len(bf_pairs))

if __name__ == "__main__":
    main()