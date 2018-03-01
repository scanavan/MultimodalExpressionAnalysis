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
6) execute_arffs
7) move_origin
8) map_phy
9) make_arffs
10) clean_bp4d
11) clean_bp4d_plus
12) make_masterfile_script

1 ~ Will separete the whole dataset at Data/BP4D+/3DFeatures/BND/DataSubset/ into smaller datasets that correspond to each of the expressions. The results will be stored in Output/Subsets/DataSubset_TX/. Therefore, the **[Full source data path]** for this function must be where the 3D data is located. This function will also create in the output directory the file "Counts.txt" which is use for validation and analysis 

2 ~ mapauto3d appens the AU values into all possible BND files.  Therefore, the **[Full source data path]** for this function must be where the AU data is located. It will attempt to match all possible files based on the AU files. However, it checks if the BND file exists before appending; so no creation happens. NOTE: in this function the variable for BND path might need to be modified.

3 ~ Makes all possible ARFF files and a text file for verification. The outputs are stored in Output/ARFF/. Since this function uses all files in the BND folder to create the ARFF files the **[Full source data path]** must be the BND folder.
At the moment it supports: BND only ARFF files (times 1 for each emotion), BND + AU (times 1 per emotion), and AU by it self (times 1 for each emotion). This function will also create the file "ValidateARFF.txt" in the output directory; the file describes how many frames of data each ARFF file has.

4 ~ Deletes all empty files in a given directory. This is mean to be used with **[Full source data path]** as the BND folder, but can be changed to anything where we want to delete empty files. The function will create in the output directory the file "deletedFrames.txt" which contains all the files that it deleted (if any) and the total number (0 if none).

5~ Makes "allframeslist.txt" in the output directory. The text file contains all the valid frames for parsing. The function first generates all possible frame file names from the AU data folder. Therefore, the **[Full source data path]** must be where the AU data is located. The function also checks that the generated file actually exists. 

**NOTE:** Please executed clean_empty command before attempting other commands (only required once of course) as empty files will break some functionalities. 

6) Makes arff_executer.bat which contains all commands to execute and save the arff files previously made. **[Full source data path]** must be path to the arff files: Output/ARFF/ and results will be saved to Output/script_results/

7) Moves all BND data to the origin using the average of the file as offset.  **[Full source data path]** must be path to original data (without BND or physiological) and will put outputs to Data/BP4D+/3DFeatures/BND/DataSubsetOrigin

8) Maps Physilogical data to Data subset that has AUs and BNDs.  **[Full source data path]** to datasaet containing both:  Data/BP4D+/3DFeatures/BND/DataSubsetOrigin

**NOTE:** please execute this function after mapping the AU.

9) makes all possible ARFF files (whole data and subsets per emotion). **[Full source data path]** must be Data/BP4D+/3DFeatures/BND/DataSubsetOrigin (Data that contains all 3 types of information) outputs go to /Output/ARFF

10) Creates a new arff file with the unwanted emotions and renames the wanted emotions.  **[Full source data path]** must be full path to the BP4D.arff file. The new arff file will be saved to ./Output/ARFF/BP4D_Filtered.arff

11) Creates a new arff file with the unwanted emotions and renames the wanted emotions.  **[Full source data path]** must be full path to the BP4D+.arff file. The new arff file will be saved to ./Output/ARFF/BP4Dplus_Filtered.arff

12) Creates a bat script to be able to train and test all filtered datasets against each other (make sure to execute 10 and 11 before doing this). The  **[Full source data path]** must be a folder that only contains the: "BP4D_Filtered.arff", "BP4Dplus_Filtered.arff", "BU4DFE.arff" files. Output will be located at ./Output/scripts/arff_executer_masterfiles.bat

