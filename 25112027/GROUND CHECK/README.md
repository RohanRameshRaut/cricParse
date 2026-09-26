# Cricket yaml file ground/venue line number 

	### PROBLEM STATEMENT
		- To check whether the ground/venue line number is at fixed position or not?
		- Is venue information mentioned on the same line number in each yaml file?

	### USAGE
	''' gcc groundCheck.c
	    ./a.out <filename.yaml> or ./a.out *.yaml(for all available .yaml file at current directory)
	'''

	### APPROACH
	* cricket yaml file may contain venue/ground information on fixed line number or different line number also, that we have to check
	  for all the available yaml files at once.

	* approach is to divide the data into lines and then match the key " venue: " with some fixed suffix and prefix pattern on every line
	  and then print the line number, similarly loop through the n(argc) files.

