
Question 1 - Seam Carving. ---------------------------------------------------

problem statement: reduce image pixels to input height and widths using seam carving algorithm. 

file : seamQ1.cpp

Compilation : g++ seamQ1.cpp  `pkg-config --cflags --libs opencv4`
input format: ./a.out 
              Image_path width Height

approach : 

    1. Used opencv to get the image as a matrix of RGB. 
    2. converted the image to grayscale using opencv functions
    3. created a energy matrix using opencv sobel function.
    4. used a dynamic programming to find the lowest energy seam from energy matrix and stored those  indices
    5. created a new image matrix which doesn't contain these seam points.
    6. repeated steps 3-5 till my width or height matches the input widht and height. 

Question 2 - Trie Harder. ---------------------------------------------------

problem statement: Design an efficient spell checker using the Trie data structure which supports certain functionalities 

file : trie_harder.cpp

Compilation : g++ trie_harder.cpp

Input Format:
• First line will contain two space-separated integers n, q which represent the number of words in the
dictionary and the number of queries to be processed, respectively.
• Next n lines will contain a single string s which represents a word in the dictionary.
• Next q lines will contain two space-separated values: an integer ai and a string ti .
– ai = 1 means Spell Check operation needs to be done on ti .
– ai = 2 means Autocomplete operation needs to be done on ti .
– ai = 3 means Autocorrect operation needs to be done on ti .

approach : 
            1. Implemented a trie data structure  .
            2. for search operation , went to each char node till i get it. if not found returned 0 else 1.
            3. to find suggestions , I went till prefix char nodes and then recursively searched all poosible words from there. 
            4. to find levdistance I written recursive code for all given possiblites. 
                all 4 conditions I will check for insertion , replace , delete , no operation .
            5. if at any place my levdistance is exceeding i return from recursion.
            5. at the end i will get all those possibile strings at distance 3
           

Question 3 - battle of banners. ---------------------------------------------------

problem statement: Your task is to maintain the silhouette efficiently and answer queries about its shape. You will be given the initial positions and heights of all banners, followed by a sequence of updates and silhouette print requests. 

file : banners.cpp

Compilation : g++ banners.cpp

Input Format:
• The first line contains an integers q —
• The next q lines each describe a query:
– Type 1 (Update): 0 left coordinate right coordinate new height Update the height of
the banner for the given stall.
– Type 2 (Silhouette): 1 Print the current silhouette.

approach : 
        1. taken all input queries and pushed them in to an arrys.
        2. Now i pushed all unique st, end points to an array and sorted it , removed duplicates and stored the count of unique points.
        3. declared segment tree using above size.(Tree compression)
        4. build the segtree using lazy propagation for updates .O(log n ) time complexity
        5. recursively searched all leaf nodes and checked for height difference.
        5. printed the position and height. 
        (Used binary search for while printing points as I had compressed the points before)

