//Author: Christopher Hill For the EEE4120F course at UCT

#include<stdio.h>
#include<CL/cl.h>
#include<iostream>
#include<fstream>
#include<string>
#include<cstring>
#include<cmath>
#include <tuple>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;


pair<double,double> launch_crack(char* words,int* word_pos, int num_words, int hash_type, char* target_hash, bool display = true)
{	
	//***step 1*** Get the platform you want to use

	// get the number of platforms on the current device
	cl_uint platformCount; //keeps track of the number of platforms you have installed on your device
	cl_platform_id *platforms;
	clGetPlatformIDs(5, NULL, &platformCount); //sets platformCount to the number of platforms

	// get all of the available platforms
	platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platformCount);
	clGetPlatformIDs(platformCount, platforms, NULL); //saves a list of platforms in the platforms variable
    
	// select the platform that you would like to use
	cl_platform_id platform = platforms[0];
	
	
	// output information of the chosen platform
	if (display){
		char* Info = (char*)malloc(0x1000*sizeof(char));
		clGetPlatformInfo(platform, CL_PLATFORM_NAME      , 0x1000, Info, 0);
		printf("Name      : %s\n", Info);
		clGetPlatformInfo(platform, CL_PLATFORM_VENDOR    , 0x1000, Info, 0);
		printf("Vendor    : %s\n", Info);
		clGetPlatformInfo(platform, CL_PLATFORM_VERSION   , 0x1000, Info, 0);
		printf("Version   : %s\n", Info);
		clGetPlatformInfo(platform, CL_PLATFORM_PROFILE   , 0x1000, Info, 0);
		printf("Profile   : %s\n", Info);
	}

	//***step 2*** get device ID must first get platform
	
	cl_device_id device; //this is your deviceID
	cl_int err;
	
	/* Access a device */
	//The if statement checks to see if the chosen platform uses a GPU, if not it setups the device using the CPU
	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	if(err == CL_DEVICE_NOT_FOUND) {
		err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
	}
	if (display){printf("Device ID = %i\n",err);}

	
	//------------------------------------------------------------------------
	
	//***Step 3*** creates a context that allows devices to send and receive kernels and transfer data
	cl_context context; //This is your contextID, the line below must just run
	context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);

	//------------------------------------------------------------------------

	//***Step 4*** get details about the kernel.cl file in order to create it (read the kernel.cl file and place it in a buffer)

	//read teh kernel file in	
	FILE *program_handle;
	program_handle = fopen("OpenCL/Kernel.cl", "r");
	

	//get kernel program size
	size_t program_size;//, log_size;
	fseek(program_handle, 0, SEEK_END);
	program_size = ftell(program_handle);
	rewind(program_handle);

	// handle the program buffer size
	char *program_buffer;//, *program_log;
	program_buffer = (char*)malloc(program_size + 1);
	program_buffer[program_size] = '\0';
	fread(program_buffer, sizeof(char), program_size, program_handle);
	fclose(program_handle);
   	

	//***Step 5*** create program from source because the kernel is in a separate file 'kernel.cl', therefore the compiler must run twice once on main and once on kernel)	

	cl_program program = clCreateProgramWithSource(context, 1, (const char**)&program_buffer, &program_size, NULL); //this compiles the kernels code

	//***Step 6*** build the program, this compiles the source code from above for the devices that the code has to run on (ie GPU or CPU)
	cl_int err3= clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (display){printf("program ID = %i\n", err3);}
	if (err3 != CL_SUCCESS) {
		char buildLog[4096];
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, NULL);
		std::cerr << "Error building program: " << err3 << std::endl;
		std::cerr << "Build log:\n" << buildLog << std::endl;
	} else {
		if (display) {
			printf("Program built successfully.\n");
		}
	}


	//***Step 7*** creates the kernel, this creates a kernel from one of the functions in the cl_program you just built

	// select the kernel function you are running
	cl_kernel kernel = clCreateKernel(program, "crackHash", &err);

	
	//***Step 8*** create command queue to the target device. This is the queue that the kernels get dispatched too, to get the the desired device.
	cl_command_queue queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, NULL);


	//***Step 9*** create data buffers for memory management between the host and the target device
	
	// start total clock timer
	clock_t total_start = clock();

	//Initialize Buffers, memory space the allows for communication between the host and the target device, (memory block) that both the host and target device can access 
	cl_mem wordsBuffer, posBuffer, tBuffer;

	// create matrixA_buffer, matrixB_buffer and output_buffer, with clCreateBuffer()
	wordsBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, (strlen(words) + 1) * sizeof(char), words, &err);
    posBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, num_words * sizeof(int), word_pos, &err);
	tBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,strlen(target_hash) * sizeof(char), target_hash, &err);

	//***Step 10*** create the arguments for the kernel (link these to the buffers set above, using the pointers for the respective buffers)
	
	// create the arguments for the kernel. Note you can create a local buffer only on the GPU as follows: clSetKernelArg(kernel, argNum, size, NULL);
	clSetKernelArg(kernel, 0, sizeof(cl_mem), &wordsBuffer);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), &posBuffer);
	clSetKernelArg(kernel, 2, sizeof(cl_mem), &tBuffer);
	clSetKernelArg(kernel, 3, sizeof(int), &num_words);
	clSetKernelArg(kernel, 4, sizeof(int), &hash_type);
	//***Step 11*** enqueue kernel, deploys the kernels and determines the number of work-items that should be generated to execute the kernel (global_size) and the number of work-items in each work-group (local_size).

	// set global_size, local_size and num_groups, in order to control the number of work item in each work group
	size_t globalSize = num_words; // Number of words
	cl_event kernel_event;

    cl_int err4 = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, NULL, 0, NULL, &kernel_event);

    // Wait for the kernel to finish
    clWaitForEvents(1, &kernel_event);


	// display a -1 to signify that the kernel has finnished executing
	cout << -1 << endl;

	// display the status of the kernels execution
	if (display && err4 == CL_SUCCESS) {
		printf("Kernel Post-Exec Check: %i \n",err4);
	}

	// Check if kernel execution was successful
    if (err4 != CL_SUCCESS && display) {
        fprintf(stderr, "Kernel execution failed: %d\n", err4);
    }

	//This command stops the program here until everything in the queue has been run
	clFinish(queue);

	// end total clock timer
	clock_t total_end = clock();

	// Get kernel execution time
	cl_ulong kernel_start_time, kernel_end_time;
	clGetEventProfilingInfo(kernel_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &kernel_start_time, NULL);
	clGetEventProfilingInfo(kernel_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &kernel_end_time, NULL);
	double kernel_time = (kernel_end_time - kernel_start_time) / 1000000000.0; // convert nanoseconds to seconds
	// double kernel_time = static_cast<double>(cl_kernel_time);
	if (display){
		printf("Kernel Execution Time: %.9f seconds\n", kernel_time);
	}

	// get the total time
	double total_time = static_cast<double>(total_end - total_start) / CLOCKS_PER_SEC;
	if (display){
		printf("Total Execution Time: %.9f seconds\n", total_time);
	}

	// combine times to an output pair
	pair<double,double> output = std::make_pair(total_time, kernel_time);

	//***Step 14*** Deallocate resources
	clReleaseKernel(kernel);
	clReleaseMemObject(wordsBuffer);
	clReleaseMemObject(posBuffer);
	clReleaseMemObject(tBuffer);
	clReleaseCommandQueue(queue);
	clReleaseProgram(program);
	clReleaseContext(context);
	return output;
}

// function used to save times to a text file
void writeTimesToFile(const std::string& filename, double total_time, double kernel_time) {
    // open the file in output mode (std::ofstream) and truncation mode (std::ios_base::trunc)
    std::ofstream outFile(filename, std::ios_base::trunc);

    // check if the file is opened successfully
    if (!outFile.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    // write the first double value followed by a newline character
    outFile << total_time << std::endl;

    // Write the second double value followed by a newline character
    outFile << kernel_time << std::endl;

    // Close the file stream
    outFile.close();
}

// function to read a dictionary into a dynamic array of strings
void readDirectory(const std::string& filename, std::vector<std::string>* directory) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
		return;
    }

    std::string line;
    while (std::getline(file, line)) {
        directory->push_back(line);
    }

    file.close();
}

pair<char*, int*> formatDictionary(std::vector<std::string>* directory, int num_words){
	// get the total number of characters
	size_t total_chars = 0;
    for (const auto& word : (*directory)) {
        total_chars += word.length();
    }

	// make char array and position array
	char* chars = new char[total_chars + 1];
	int* positions = new int[num_words + 1];

	char* char_pos = chars;
	int i = 0;
    for (const auto& str : (*directory)) {
        std::memcpy(char_pos, str.c_str(), str.length());
		positions[i] = char_pos-chars;
        char_pos += str.length();
		i++;
    }
	positions[i] = char_pos-chars;

	chars[total_chars] = '\0';

	pair<char*, int*> out = std::make_pair(chars, positions);
	return out;
}

int main(int argc, char* argv[]){
	std::string filename = argv[1]; // read in dictionary file path from arguments
	std::string hash_type = argv[2]; // read in hash type

	// read in target_hash
	std::string target_hash = argv[3]; 

	//printf("Hash Length: %zu\n",target_hash.length());
	char* c_target_hash = new char[target_hash.length()+1];
	std::memcpy(c_target_hash, target_hash.c_str(), target_hash.length());
	c_target_hash[target_hash.length()] = '\0';


	// get extra parameters if requested
	std::string times_path = "results/times.txt"; 
	bool display = false;
	if (argc > 4){
		for(int argi = 4; argi < argc; argi++){
			if (strcmp(argv[argi], "-d") == 0){
				display = true;
			}else if(strcmp(argv[argi], "-s") == 0){
				argi++;
				times_path = argv[argi];
			}
		}
	}
	
	// read the dictionary
	std::vector<std::string>* directory = new vector<std::string>();
    readDirectory(filename, directory);

	// format the disctionary to opencl desired style
	int num_words = static_cast<int>(directory->size());
	pair<char*, int*> c_dictionary = formatDictionary(directory,num_words);
	delete directory;

	// map hash type name to designated number
	int i_hash_type = -1;
	if (hash_type == "MD5" || hash_type == "md5"){ // MD5 = 0
		i_hash_type = 0;
	}else if (hash_type == "SHA-1" || hash_type == "sha1" || hash_type == "sha-1"){ // SHA-1 = 1
		i_hash_type = 1;
	}

	// ensure that valid hashing function has been selected
	if (i_hash_type == -1){
		cout << "Unsupported hashing function " + hash_type + "." << endl;
	}
	

	// launch the cracker
	pair<double,double> times = launch_crack(c_dictionary.first, c_dictionary.second, num_words,i_hash_type,c_target_hash,display);

	// write the output times to the outpur text file
	writeTimesToFile(times_path,times.first,times.second);

	// delete any dynamic stuff
	delete [] c_dictionary.first;
	delete [] c_dictionary.second;
	delete [] c_target_hash;
	return 0;
}