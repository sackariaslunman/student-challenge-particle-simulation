# Prototyping

I began by making a rough prototype in the prototype.ipynb file. I coded in python to speed up prototyping and to develop the algorithm. The problem of finding close pairs is an old one and after some googling I came up with a solution. The algorithm sorts the points in the x-axis and starts doing a recursive loop. Pseudo-code:

    function find_close_pairs_recursion:

        if the size of the array is lower than the cutoff value:
            # Brute force the solution
            return brute_force

        if one of the ends of the subarray is within the min distance to the mid point:
            sort the subarray in the next axis
            return find_close_pairs_recursion # run recursion again searching on the next axis
        
        # Find the left-most point that is within the min distance in the subarray with binary search
        mid_lo = binary_search(from first to middle point)

        # Find the right-most point that is within the min distance in the subarray with binary search
        mid_hi = binary_search(from mid to last point)

        # Check if there are pairs, between points to the left and right of the mid point, that arewithin the min distance 
        mid_pairs = brute_force(from mid_lo to mid, from mid to mid_hi)

        # Find pairs within the min distance in the left and right half
        lo_pairs = find_close_pairs(left half)
        hi_pairs = find_close_pairs(right half)

        return lo_pairs + mid_pairs + hi_pairs

Roughly, the algorithm recursively searches for pairs in the left half and right half. There could also be pairs in the middle between the two halves so the algorithm checks for pairs within the min distance of the middle point. To find this range, binary search is used to lower the complexity from a worst case of O(N) to O(log(N)).

If the recursion has recursed down to a sub array lower than an arbitrary cutoff value, a solution is brute forced.

If the left most or right most point in the subarray is within the minimum distance of the mid point, the sub array is sorted on the next axis, for example jumping from the x-axis to the y-axis. This works well when the dataset of points are very close to each other and there needs to be a lot of recursion.

The code runs using different levels of dimension depths, i.e. if you have a dim_depth of 1, the code will only sort using one axis, the x-axis. Dim_depth = 3 will resort the points on the y-axis after a while, and then the z-axis after a while.

In the end I got down to 5 seconds with the python code using dim_depth of 3, which was way quicker than dim_depth = 1.

# Refining

To make the code faster I rewrote the program in C++. I used ref types to avoid copying the data and I compiled the code with the -O3 optimization flag. To compile the code run `g++ -o main.exe main.cpp -O3 -std=c++20` (you can use another compiler than g++). The main.exe file is also in the repo, compiled for Windows. To run the code run `main.exe 0.05 5` where 0.05 is the min distance and 5 is the cutoff value. You can change these however you like, but they need to be positive numbers. The algorithm runs 3 times using different dim_depths.

In the end I got down to a run time of 1200 ms, but for some reason using dim_depth = 1 is faster than dim_depth = 3.

# Conclusion

After some experimentation and looking at the code on and off for a week, I think I reached a pretty good level, but there could definitely be some improvements in the code. I think the C++ program can run way faster but I'm not sure where the bottleneck is. Will have to investigate further. Still, the algorithm is way faster than brute forcing the entire list of 16124 points, which takes ~4 minutes