#include "xcl2.hpp"
#include <vector>
#include <math.h>
#include <stdio.h>

#define N 2
#define M 3
#define M2 1

using std::vector;

static const std::string error_message =
    "Error: Result mismatch:\n"
    "i = %d CPU result = %d Device result = %d\n";

int main(){
	double Tr,Ti;
	char i,j,K,L,I,J;
	char I1,I2,I3,I4,JR,J1,J2;
	char KF,K1,K2;

	// Creates a vector of [N][N] elements
	vector<double,aligned_allocator<double>> Fr(N)(N);
	vector<double,aligned_allocator<double>> Fi(N)(N);
  vector<double,aligned_allocator<double>> Fr_results(N)(N);
  vector<double,aligned_allocator<double>> Fi_results(N)(N);

	//input DATA
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

	// The get_xil_devices will return vector of Xilinx Devices
  //ザイリンクスデバイスのベクトルを返す。
	std::vector<cl::Device> devices = xcl::get_xil_devices();
	cl::Device device = devices[0];

	//Creating Context and Command Queue for selected Device
  //選択したデバイスのcontextとcommand queueを作成する。
	cl::Context context(device);
	cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE);
	std::string device_name = device.getInfo<CL_DEVICE_NAME>();
	std::cout << "Found Device=" << device_name.c_str() << std::endl;

	// import_binary() command will find the OpenCL binary file created using the
	// xocc compiler load into OpenCL Binary and return as Binaries
	// OpenCL and it can contain many functions which can be executed on the
	// device.
  //このコマンドはopenclバイナリにロードされたxoccコンパイラを用いて
  //作られたバイナリファイルを見つけ、binaries openclとして
  //返す。
	std::string binaryFile = xcl::find_binary_file(device_name,"FFT2JT");
	cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
	devices.resize(1);
	cl::Program program(context, devices, bins);

  // These commands will allocate memory on the FPGA. The cl::Buffer objects can
  // be used to reference the memory locations on the device. The cl::Buffer
  // object cannot be referenced directly and must be passed to other OpenCL
  // functions.
  //FPGA上のメモリ割り当てを行う。
  cl::Buffer buffer_Fr(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
				size_in_bytes, Fr.data());
  cl::Buffer buffer_Fi(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
				size_in_bytes, Fi.data());
  cl::Buffer buffer_Fr_results(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
				size_in_bytes, Fr_results.data());
  cl::Buffer buffer_Fi_results(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
      	size_in_bytes, Fi_results.data());

  //Separate Read/write Buffer vector is needed to migrate data between host/device
  //read/write buffer vectorを分けるにはhost/device間のデータ転送が必要。
  std::vector<cl::Memory> inBufVec, outBufVec;
  inBufVec.push_back(buffer_Fr);
  inBufVec.push_back(buffer_Fi);
  outBufVec.push_back(buffer_Fr_results);
  outBufVec.push_back(buffer_Fi_results);

  // These commands will load the source_a and source_b vectors from the host
  // application and into the buffer_a and buffer_b cl::Buffer objects. The data
  // will be be transferred from system memory over PCIe to the FPGA on-board
  // DDR memory.
  //これらのコマンドは、ベクトルをホストアプリケーションからロードし、
  //Bufferオブジェクトに代入しする。
  //データは、PCIeを介してシステムメモリからFPGAのオンボードDDRメモリに転送される。
  q.enqueueMigrateMemObjects(inBufVec,0/* 0 means from host*/);

  // This call will extract a kernel out of the program we loaded in the
  // previous line. A kernel is an OpenCL function that is executed on the
  // FPGA. This function is defined in the src/vetor_addition.cl file.
  //この呼び出しは、前の行でロードしたプログラムからカーネルを抽出する。
  //カーネルとは、FPGA上で実行されるOpenCL関数である。
  //この関数はsrc / vetor_addition.clファイルに定義されています。
  cl::Kernel krnl_FFT2JT(program,"FFT2JT");

  //set the kernel Arguments
  //カーネル設定。
  int narg=0;
  krnl_FFT2JT.setArg(narg++,buffer_Fr);
  krnl_FFT2JT.setArg(narg++,buffer_Fi);
  krnl_FFT2JT.setArg(narg++,buffer_Fr_results);
  krnl_FFT2JT.setArg(narg++,buffer_Fi_results);

  //Launch the Kernel
  //カーネル実行。
  q.enqueueTask(krnl_FFT2JT);

  // The result of the previous kernel execution will need to be retrieved in
  // order to view the results. This call will write the data from the
  // buffer_result cl_mem object to the source_results vector
  //この結果を表示するには、前回のカーネル実行の結果を取得する必要があります。
  //この呼び出しは、buffer_result cl_memオブジェクトから
  //source_resultsベクトルにデータを書き込みます。
  q.enqueueMigrateMemObjects(outBufVec,CL_MIGRATE_MEM_OBJECT_HOST);
  q.finish();

	for (i=0; i<N; i++){
  	for (j=0; j<N; j++){

      printf("Fr[%d][%d]=%lf,Fi[%d][%d]=%lf\n",i,j,Fr_results[i][j],i,j,Fi_results[i][j]);

  	}
	}

}
