# MultimodalExpressionAnalysis
Research on 2D, 3D, physiological, AU, and thermal face expression analysis

- Path to AU data = Data/AUCoding/AU_OCC/
- Path to 3D/BND data = Data/BP4D+/3DFeatures/BND/DataSubset/

The code can be executed from the command line as follows:

./facialExpressions [Full source data path] [function]

There are 3 available functions:
1) create_subsets
2) mapauto3d
3) make_arffs

1 ~ Will separete the whole dataset at Data/BP4D+/3DFeatures/BND/DataSubset/ into smaller datasets that correspond to each of the expressions. The results will be stored in Output/Subsets/DataSubset_TX/. Therefore, the [Full source data path] for this function must be where the 3D data is located.

2 ~ mapauto3d appens the AU values into all possible BND files.  Therefore, the [Full source data path] for this function must be where the AU data is located. It will attempt to match all possible files based on the AU files. However, it checks if the BND file exists before appending; so no creation happens. NOTE: in this function the variable for BND path might need to be modified.

3 ~ Makes all possible ARFF files and a text file for verification. The outputs are stored in Output/ARFF/. Since this function uses all files in the BND folder to create the ARFF files the [Full source data path] must be the BND folder.
At the moment it supports: BND only ARFF files (times 1 for each emotion), BND + AU (times 1 per emotion), and AU by it self (times 1 for each emotion).
