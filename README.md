# MultimodalExpressionAnalysis
Research on 2D, 3D, physiological, AU, and thermal face expression analysis

- Path to AU data = Data/AUCoding/AU_OCC/
- Path to 3D/BND data = Data/BP4D+/3DFeatures/BND/DataSubset/

The code can be executed from the command line as follows:

**./facialExpressions [Full source data path] [function]**

There are 5 available functions:

1) create_subsets
2) mapauto3d
3) make_arffs
4) clean_empty
5) make_frame_list

1 ~ Will separete the whole dataset at Data/BP4D+/3DFeatures/BND/DataSubset/ into smaller datasets that correspond to each of the expressions. The results will be stored in Output/Subsets/DataSubset_TX/. Therefore, the **[Full source data path]** for this function must be where the 3D data is located. This function will also create in the output directory the file "Counts.txt" which is use for validation and analysis 

2 ~ mapauto3d appens the AU values into all possible BND files.  Therefore, the **[Full source data path]** for this function must be where the AU data is located. It will attempt to match all possible files based on the AU files. However, it checks if the BND file exists before appending; so no creation happens. NOTE: in this function the variable for BND path might need to be modified.

3 ~ Makes all possible ARFF files and a text file for verification. The outputs are stored in Output/ARFF/. Since this function uses all files in the BND folder to create the ARFF files the **[Full source data path]** must be the BND folder.
At the moment it supports: BND only ARFF files (times 1 for each emotion), BND + AU (times 1 per emotion), and AU by it self (times 1 for each emotion). This function will also create the file "ValidateARFF.txt" in the output directory; the file describes how many frames of data each ARFF file has.

4 ~ Deletes all empty files in a given directory. This is mean to be used with **[Full source data path]** as the BND folder, but can be changed to anything where we want to delete empty files. The function will create in the output directory the file "deletedFrames.txt" which contains all the files that it deleted (if any) and the total number (0 if none).

5~ Makes "allframeslist.txt" in the output directory. The text file contains all the valid frames for parsing. The function first generates all possible frame file names from the AU data folder. Therefore, the **[Full source data path]** must be where the AU data is located. The function also checks that the generated file actually exists. 

**NOTE:** Please executed clean_empty command before attempting other commands (only required once of course) as empty files will break some functionalities. 
