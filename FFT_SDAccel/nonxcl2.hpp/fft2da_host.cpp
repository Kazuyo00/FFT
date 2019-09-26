#include <CL/cl.h>
#include <cstdio>
#include <fstream>
#include <iosfwd>
#include <string>
#include <vector>
#include <iostream>

//=======================

//#include "xcl2.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/resource.h>

//=======================

using std::ifstream;
using std::streamsize;
using std::string;
using std::vector;
using std::ios;

//==============================

#define N 8 //処理画素数
#define M 3
#define M2 1

static const std::string error_message =
    "Error: Result mismatch:\n"
    "i = %d CPU result = %d Device result = %d\n";

// Creates a vector of [N*N] elements
/*std::vector<std::vector<double> > Fr;
std::vector<std::vector<double> > Fi;
std::vector<std::vector<double> > Fr_results;
std::vector<std::vector<double> > Fi_results;*/

double Fr[N][N];
double Fi[N][N];
double Fr_results[N][N];
double Fi_results[N][N];

//==============================

#define ERROR_CASE(err) \
    case err:           \
        return #err;    \
        break

const char *error_string(cl_int error_code) {
    switch (error_code) {
        ERROR_CASE(CL_SUCCESS);
        ERROR_CASE(CL_DEVICE_NOT_FOUND);
        ERROR_CASE(CL_DEVICE_NOT_AVAILABLE);
        ERROR_CASE(CL_COMPILER_NOT_AVAILABLE);
        ERROR_CASE(CL_MEM_OBJECT_ALLOCATION_FAILURE);
        ERROR_CASE(CL_OUT_OF_RESOURCES);
        ERROR_CASE(CL_OUT_OF_HOST_MEMORY);
        ERROR_CASE(CL_PROFILING_INFO_NOT_AVAILABLE);
        ERROR_CASE(CL_MEM_COPY_OVERLAP);
        ERROR_CASE(CL_IMAGE_FORMAT_MISMATCH);
        ERROR_CASE(CL_IMAGE_FORMAT_NOT_SUPPORTED);
        ERROR_CASE(CL_BUILD_PROGRAM_FAILURE);
        ERROR_CASE(CL_MAP_FAILURE);
        ERROR_CASE(CL_INVALID_VALUE);
        ERROR_CASE(CL_INVALID_DEVICE_TYPE);
        ERROR_CASE(CL_INVALID_PLATFORM);
        ERROR_CASE(CL_INVALID_DEVICE);
        ERROR_CASE(CL_INVALID_CONTEXT);
        ERROR_CASE(CL_INVALID_QUEUE_PROPERTIES);
        ERROR_CASE(CL_INVALID_COMMAND_QUEUE);
        ERROR_CASE(CL_INVALID_HOST_PTR);
        ERROR_CASE(CL_INVALID_MEM_OBJECT);
        ERROR_CASE(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
        ERROR_CASE(CL_INVALID_IMAGE_SIZE);
        ERROR_CASE(CL_INVALID_SAMPLER);
        ERROR_CASE(CL_INVALID_BINARY);
        ERROR_CASE(CL_INVALID_BUILD_OPTIONS);
        ERROR_CASE(CL_INVALID_PROGRAM);
        ERROR_CASE(CL_INVALID_PROGRAM_EXECUTABLE);
        ERROR_CASE(CL_INVALID_KERNEL_NAME);
        ERROR_CASE(CL_INVALID_KERNEL_DEFINITION);
        ERROR_CASE(CL_INVALID_KERNEL);
        ERROR_CASE(CL_INVALID_ARG_INDEX);
        ERROR_CASE(CL_INVALID_ARG_VALUE);
        ERROR_CASE(CL_INVALID_ARG_SIZE);
        ERROR_CASE(CL_INVALID_KERNEL_ARGS);
        ERROR_CASE(CL_INVALID_WORK_DIMENSION);
        ERROR_CASE(CL_INVALID_WORK_GROUP_SIZE);
        ERROR_CASE(CL_INVALID_WORK_ITEM_SIZE);
        ERROR_CASE(CL_INVALID_GLOBAL_OFFSET);
        ERROR_CASE(CL_INVALID_EVENT_WAIT_LIST);
        ERROR_CASE(CL_INVALID_EVENT);
        ERROR_CASE(CL_INVALID_OPERATION);
        ERROR_CASE(CL_INVALID_GL_OBJECT);
        ERROR_CASE(CL_INVALID_BUFFER_SIZE);
        ERROR_CASE(CL_INVALID_MIP_LEVEL);
        ERROR_CASE(CL_INVALID_GLOBAL_WORK_SIZE);
        ERROR_CASE(CL_COMPILE_PROGRAM_FAILURE);
        ERROR_CASE(CL_LINKER_NOT_AVAILABLE);
        ERROR_CASE(CL_LINK_PROGRAM_FAILURE);
        ERROR_CASE(CL_DEVICE_PARTITION_FAILED);
        ERROR_CASE(CL_KERNEL_ARG_INFO_NOT_AVAILABLE);
        ERROR_CASE(CL_INVALID_PROPERTY);
        ERROR_CASE(CL_INVALID_IMAGE_DESCRIPTOR);
        ERROR_CASE(CL_INVALID_COMPILER_OPTIONS);
        ERROR_CASE(CL_INVALID_LINKER_OPTIONS);
        ERROR_CASE(CL_INVALID_DEVICE_PARTITION_COUNT);
        default:
            printf("Unknown OpenCL Error (%d)\n", error_code);
            break;
    }
    return nullptr;
}

//xcl2.hppの代わりにこのようにバイナリファイルを読み込む必要がある。
vector<unsigned char> readBinary(const std::string &fileName) {
    ifstream file(fileName, ios::binary | ios::ate);
    if (file) {
        file.seekg(0, ios::end);
        streamsize size = file.tellg();
        file.seekg(0, ios::beg);
        vector<unsigned char> buffer(size);
        file.read((char *)buffer.data(), size);
        return buffer;
    } else {
        fprintf(stderr, "%s not found\n", fileName.c_str());
        exit(-1);
    }
    return std::vector<unsigned char>(0);
}


int main(int argc, char **argv) {
	double Tr,Ti;
	int i,j,K,L,I,J;
	int I1,I2,I3,I4,JR,J1,J2;
	int KF,K1,K2;

	size_t mem_size = sizeof(double)*N*N;

	printf ( "Check1\n" );

	//vector of [N*N] elementsの初期化
/*	Fr = vector<vector<double>>(N, vector<double>(N, 0));
	Fi = vector<vector<double>>(N, vector<double>(N, 0));
	Fr_results = vector<vector<double>>(N, vector<double>(N, 0));
	Fi_results = vector<vector<double>>(N, vector<double>(N, 0));*/

	printf ( "Check2, end initialization\n" );

	for (i=0;i<N;i++){
		for (j=0;j<N;j++){
			if(i<3 && j<2)
        Fr[i][j]=1.0;
        Fi[i][j]=0.0;
			if(i>=3 || j>=2)
        Fr[i][j]=0.0;
        Fi[i][j]=0.0;
		}
	}
	printf ( "Check3, end input data\n" );

	/*for (i=0; i<N; i++){
				for (j=0; j<N; j++){

					printf("Fr_input[%d][%d]=%lf,Fi_input[%d][%d]=%lf\n///////////////////////////////////////\n"
							,i,j,Fr[i][j],i,j,Fi[i][j]);

				}
	}*/

	//start bit reverse
	for (L=0;L<M2;L++){
		I1=1<<L;
		I2=N/I1;
		I3=I2/2;
		I4=I3/2;

		for(I=0;I<I1;I++){
			JR=I*I2;
			J1=JR+I1;
			J2=JR+I3;

			for (J=0;J<I4;J++){
				KF=(J/I1)*I1+J;
				K1=KF+J1;
			  K2=KF+J2;

				for (K=0;K<N;K++){
					Tr=Fr[K][K1];
					Ti=Fi[K][K1];
					Fr[K][K1]=Fr[K][K2];
					Fi[K][K1]=Fi[K][K2];
					Fr[K][K2]=Tr;
					Fi[K][K2]=Ti;
				}
				for (K=0;K<N;K++){
					Tr=Fr[K1][K];
					Ti=Fi[K1][K];
					Fr[K1][K]=Fr[K2][K];
					Fi[K1][K]=Fi[K2][K];
					Fr[K2][K]=Tr;
					Fi[K2][K]=Ti;
				}
			}
		}
	}
	//end bit reverse

	/*for (i=0; i<N; i++){
			for (j=0; j<N; j++){

				printf("Fr_reverse[%d][%d]=%lf,Fi_reverse[%d][%d]=%lf\n///////////////////////////////////////\n"
						,i,j,Fr[i][j],i,j,Fi[i][j]);

			}
	}*/

	//input DATA
	/*	for (i=0;i<N;i++){
			for (j=0;j<N;j++){
				if(i<3 && j<2)
	        Fr[j + i*N]=1.0;
	        Fi[j + i*N]=0.0;
				if(i>=3 || j>=2)
	        Fr[j + i*N]=0.0;
	        Fi[j + i*N]=0.0;
			}
		}

		//start bit reverse
		for (L=0;L<M2;L++){
			I1=1<<L;
			I2=N/I1;
			I3=I2/2;
			I4=I3/2;

			for(I=0;I<I1;I++){
				JR=I*I2;
				J1=JR+I1;
				J2=JR+I3;

				for (J=0;J<I4;J++){
					KF=(J/I1)*I1+J;
					K1=KF+J1;
				  K2=KF+J2;

					for (K=0;K<N;K++){
						 Tr=Fr[K+K1*N];
						 Ti=Fi[K+K1*N];
						 Fr[K+K1*N]=Fr[K+K2*N];
						 Fi[K+K1*N]=Fi[K+K2*N];
						 Fr[K+K2*N]=Tr;
						 Fi[K+K2*N]=Ti;
					}
						for (K=0;K<N;K++){
							Tr=Fr[K1+K*N];
							Ti=Fi[K1+K*N];
							Fr[K1+K*N]=Fr[K2+K*N];
							Fi[K1+K*N]=Fi[K2+K*N];
							Fr[K2+K*N]=Tr;
							Fi[K2+K*N]=Ti;
						}
				}
			}
		}
//end bit reverse
 */
	printf ( "Check4, end bit reverse\n" );

  if (argc != 2) {
      printf("Usage: %s "
             "./xclbin/vector_addition.<emulation_mode>.<dsa>.xclbin\n",
             argv[0]);
      return EXIT_FAILURE;
  }
  //static const int elements = 128;
  char *binary_file_path = argv[1];

  // Error handling in OpenCL is performed using the cl_int specifier. OpenCL
  // functions either return or accept pointers to cl_int types to indicate if
  // an error occured.
  cl_int err;
  cl_event read1,read2,calculation,write1,write2;
  cl_uint num_platforms;
  if ((err = clGetPlatformIDs(0, nullptr, &num_platforms))) {
      printf(
          "Fatal Error calling clGetPlatformIDs: %s\n"
          "This can be caused by an invalid installation of the OpenCL "
          "runtime. Please make sure the installation was successful.\n",
          error_string(err));
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  }

  if (num_platforms == 0) {
      printf(
          "No platforms were found. This could be caused because the OpenCL "
          "icd was not installed in the /etc/OpenCL/vendors directory.\n");
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  }

  vector<cl_platform_id> platforms(num_platforms + 1);
  if ((err = clGetPlatformIDs(num_platforms, platforms.data(), nullptr))) {
      printf("Error: Failed to find an OpenCL platform!\n");
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  }

  string platform_name(1024, '\0'); //<-1024という数字は何??
  size_t actual_size = 0;
  if ((err = clGetPlatformInfo(platforms[0], CL_PLATFORM_NAME,
                               platform_name.size(),
                               (void *)platform_name.data(), &actual_size))) {
      printf("Error: Could not determine platform name!\n");
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  }
  printf("Platform Name: %s\n", platform_name.c_str());

  if ((err = clGetPlatformInfo(platforms[0], CL_PLATFORM_VERSION,
                               platform_name.size(),
                               (void *)platform_name.data(), &actual_size))) {
      printf("Error: Could not determine platform OpenCL Version!\n");
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  }
  printf("Platform Name: %s\n", platform_name.c_str());

  cl_device_id device_id = 0;
  if ((err = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 1, &device_id,
                            nullptr))) {
      printf(
          "Fatal Error calling clGetDeviceIDs: %s\n"
          "Unexpected error getting device IDs. This may happen if you are "
          "Targeting hardware or software emulation and the "
          "XCL_EMULATION_MODE environment variable is not set. Also makeyou "
          "have set the you have run the emconfigutil to setup the emulation "
          "environment.\n\n",
          error_string(err));
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  }

  cl_context_properties props[3] = {CL_CONTEXT_PLATFORM,
                                    (cl_context_properties)platforms[0], 0};
  // clCreate* function calls return the object so they return error codes
  // using
  // pointers to cl_int as their last parameters

  cl_context context = clCreateContext(props, 1, &device_id, nullptr, nullptr, &err);

  cl_command_queue command_queue = clCreateCommandQueue(
      context, device_id, CL_QUEUE_PROFILING_ENABLE, &err);

  if (err) {
      printf(
          "Fatal Error calling clCreateCommandQueue: %s\n"
          "Unexpected error creating a command queue.\n\n",
          error_string(err));
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  }

  // Loading the file
  vector<unsigned char> binary = readBinary(binary_file_path);
  size_t binary_size = binary.size();
  const unsigned char *binary_data = binary.data();

  cl_program program = clCreateProgramWithBinary(context, 1, &device_id, &binary_size,
                                      &binary_data, NULL, &err);
  if (err) {
      printf(
          "Fatal Error calling clCreateProgramWithBinary: %s\n"
          "Unexpected error creating a program from binary. Make sure you "
          "executed this program with the correct path to the binary.\n\n",
          error_string(err));
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  }

  cl_kernel kernel = clCreateKernel(program, "FFT2Da", &err);

  if (err) {
      printf(
          "Fatal Error calling clCreateKernel: %s\n"
          "Unexpected error when creating kernel. Make sure you passed the "
          "correct binary into the executable of this program.\n\n",
          error_string(err));
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  }

///////////////////////////////////////////////////////////////////////////

 /* cl_mem clCreateBuffer(cl_context context,//OpenCLのコンテキストを指定
  							cl_mem_flags flags,//フラグ
  							size_t size, //確保するメモリのバイトサイズ
  							void *host_ptr,//CPU側のデータへのポインタ
  							cl_int *errcode_ret)//エラーコードを拾う時に使用 */

  cl_mem d_Fr = clCreateBuffer(context, CL_MEM_READ_ONLY, mem_size, nullptr, &err);
  if (err) {
      printf("Fatal Error calling clCreateBuffer: %s\n", error_string(err));
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  }

  cl_mem d_Fi = clCreateBuffer(context, CL_MEM_READ_ONLY, mem_size, nullptr, &err);
  if (err) {
      printf("Fatal Error calling clCreateBuffer: %s\n", error_string(err));
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  }

  cl_mem d_Fr_results = clCreateBuffer(context, CL_MEM_READ_WRITE, mem_size, nullptr, &err);
  if (err) {
      printf("Fatal Error calling clCreateBuffer: %s\n", error_string(err));
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  }

  cl_mem d_Fi_results = clCreateBuffer(context, CL_MEM_READ_WRITE, mem_size, nullptr, &err);
  if (err) {
      printf("Fatal Error calling clCreateBuffer: %s\n", error_string(err));
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  }

///////////////////////////////////////////////////////////////////////////

//CPU ⇒ Kernel Device のデータ転送
 /*clEnqueueWriteBuffer 			(	cl_command_queue  //command_queue、
   										cl_mem  //バッファ、
   										cl_bool  //blocking_write、
   										size_t  //オフセット、
   										size_t  //cb、
   										const void  //ptr、
   										cl_uint  //num_events_in_wait_list、
   										const cl_event  //event_wait_list、
   										cl_event  //イベント)*/

  if ((err = clEnqueueWriteBuffer(command_queue, d_Fr, CL_TRUE, 0, mem_size,
		  Fr, 0, nullptr, &write1))) {
      printf("Fatal Error calling clEnqueueWriteBuffer: %s\n",
             error_string(err));
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  }

  clWaitForEvents(1,&write1);

  if ((err = clEnqueueWriteBuffer(command_queue, d_Fi, CL_TRUE, 0, mem_size,
		  Fi, 0, nullptr, &write2))) {
      printf("Fatal Error calling clEnqueueWriteBuffer: %s\n",
             error_string(err));
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  }

  clWaitForEvents(1,&write2);

///////////////////////////////////////////////////////////////////////////

  if ((err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&d_Fr))) {
      printf("Fatal Error calling clSetKernelArg: %s\n", error_string(err));
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  }
  if ((err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&d_Fi))) {
      printf("Fatal Error calling clSetKernelArg: %s\n", error_string(err));
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  }
  if ((err = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&d_Fr_results))) {
      printf("Fatal Error calling clSetKernelArg: %s\n", error_string(err));
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  }
  if ((err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&d_Fi_results))) {
      printf("Fatal Error calling clSetKernelArg: %s\n", error_string(err));
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  }

  ///////////////////////////////////////////////////////////////////////////
  printf ( "Check5 finish write\n" );

  if ((err = clEnqueueTask(command_queue,
  				  	  	  	  	   	kernel,
  									0,
  									nullptr,
  									&calculation)))
  		{
  		      printf("Fatal Error calling clEnqueueNDRangeKernel: %s\n",
  		             error_string(err));
  		      printf("TEST FAILED\n");
  		      exit(EXIT_FAILURE);
  		}

  //
  //計算終了を待つため(同期を取るため)
  clWaitForEvents(1,&calculation);

  printf ( "Check6 finish kernel\n" );
  ///////////////////////////////////////////////////////////////////////////

  //Kernel Device ⇒ CPU のデータ転送
  /*clEnqueueReadBuffer(cl_command_queue command_queue,//コマンドキュー
  	  	  	  	  	  	  cl_mem buffer,//バッファオブジェクト
  	  	  	  	  	  	  cl_bool blocking_read,//ブロッキングモードを指定(CL_TRUE or CL_FALSE)
  	  	  	  	  	  	  size_t offset,//データの開始位置のオフセット
  	  	  	  	  	  	  size_t cb,//データのバイトサイズ
  	  	  	  	  	  	  void *ptr,//読み出し先。CPU側の格納用配列データのアドレス
  	  	  	  	  	  	  cl_uint num_events_in_wait_list,//イベントのリスト。とりあえず「0」でいい
  	  	  	  	  	  	  const cl_event *event_wait_list,//イベントリストの数。とりあえず「NULL」でいい
  	  	  	  	  	  	  cl_event *event)//とりあえずNULLでいい*/

  if ((err = clEnqueueReadBuffer(command_queue, d_Fr_results, CL_TRUE, 0, mem_size,
									  Fr_results, 0, nullptr, &read1)))
  	  {
      printf("Fatal Error calling clEnqueueWriteBuffer: %s\n",
             error_string(err));
      printf("TEST FAILED\n");
      exit(EXIT_FAILURE);
  	  }

  clWaitForEvents(1,&read1);

  if ((err = clEnqueueReadBuffer(command_queue, d_Fi_results, CL_TRUE, 0, mem_size,
  									  Fi_results, 0, nullptr, &read2)))
  	  {
        printf("Fatal Error calling clEnqueueWriteBuffer: %s\n",
               error_string(err));
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
  	  }

  clWaitForEvents(1,&read2);

  printf ( "Check7 finish read\n" );
  ///////////////////////////////////////////////////////////////////////////


  for (i=0; i<N; i++){
	  for (j=0; j<N; j++){

			printf("Fr[%d][%d]=%lf,Fi[%d][%d]=%lf\n"
					,i,j,Fr_results[i][j],i,j,Fi_results[i][j]);

	  }
  }

  cl_ulong start1, end1, start2, end2, start3, end3, start4, end4, start5, end5;

  clGetEventProfilingInfo(write1, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start1, NULL);
  clGetEventProfilingInfo(write1, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end1, NULL);
  clGetEventProfilingInfo(write2, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start2, NULL);
  clGetEventProfilingInfo(write2, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end2, NULL);

  clGetEventProfilingInfo(calculation, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start3, NULL);
  clGetEventProfilingInfo(calculation, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end3, NULL);

  clGetEventProfilingInfo(read1, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start4, NULL);
  clGetEventProfilingInfo(read1, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end4, NULL);
  clGetEventProfilingInfo(read2, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start5, NULL);
  clGetEventProfilingInfo(read2, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end5, NULL);

  printf("Write TIME Prof = %lu[ns]\n",((end1-start1)+(end2-start2)));
  printf("Kernel TIME Prof = %lu[ns]\n",(end3-start3));
  printf("Read TIME Prof = %lu[ns]\n\n",((end4-start4)+(end5-start5)));
  printf("WKR TIME Prof = %lu[ns]\n\n",(((end1-start1)+(end2-start2))+(end3-start3)+((end4-start4)+(end5-start5))));

  printf ( "Check9 finish all\n" );
  clReleaseDevice(device_id);
  clReleaseMemObject(d_Fr);
  clReleaseMemObject(d_Fi);
  clReleaseMemObject(d_Fr_results);
  clReleaseMemObject(d_Fi_results);
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseCommandQueue(command_queue);
  clReleaseContext(context);
  clReleaseEvent(read1);
  clReleaseEvent(read2);
  clReleaseEvent(calculation);
  clReleaseEvent(write1);
  clReleaseEvent(write2);


  return 0;

}

