# PROBLEM STATEMENT
	- Create Objects to represent cricket match data from the provided .yaml file format.
	- Verify whether the given data in .yaml file is following the .yaml file format or not.
	- Display the match summary in tabular format.

# USAGE
	 '''
	  gcc <filename.c> -lncurses
	  ./a.out <filename.yaml>

	  '''

	  - <tag> -lncurses is used from ncurses library to display the objects on tabular format

# APPROACH
	  - Divide the file content into lines
	  - Validate the allowed bytes and not allowed bytes
	  - Identified the total number of <keys> used throughout the all yaml files
	  - All the <keys> are then indexed to match the patterns in the file and get the required information
	  - Different flag and array like approaches are used to minimize the code
	  - ncurses library is used to display the summary objects in tabular format
