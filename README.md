
# FNV Hash function - spell checker

This is a spell checking program which brings you suggestions for misspelled words. User can modify the program data by adding a word to the dictionary using the third command line argument. Moreover, the structure of the program is user friendly and gives out results in a time efficient manner.

## But what is so special about this program?
Instead of using the usual method of multiplying 101 with each character and multiplying it (https://stackoverflow.com/questions/98153/whats-the-best-hashing-algorithm-to-use-on-a-stl-string-when-using-hash-map), this algorithm uses the Fowler Null Von hashing method to compute the hash value. The advantages of using FNV Hash is that we less collisions per insertions. Hence, the time complexity for each insertion is less than that required for the 101 Paul Larson method. For more information about the FNV Hash - https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

## How to use the program?
First, go to file manager on your respective platform and make two text files. One named words.txt and the other is your input file. In the words.txt, you will have to copy all the valid dictionary words and paste them on the words.txt file. After this, type in your inpit text in the input file and open the location in your respective platform terminal. Now, to run the program, download the given executable on the respository and run it on the command-line using three arguments - 
* 1 - Name of the dictionary file
* 2 - Name of the input file
* 3 - Option to add the incorrect words in the dictionary or ignore them

Hence, it would look something like this > ./check words.txt <input file name> <add/ignore>
