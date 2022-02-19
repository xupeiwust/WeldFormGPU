#include "Domain_d.cuh"
#include "Functions.cuh"
#include "Domain.h"
//Allocating from host
namespace SPH {
void Domain_d::SetDimension(const int &particle_count){
	//Allocae arrays (as Structure of arryays, SOA)

	cudaMalloc((void **)&x, particle_count * sizeof (double3));
	cudaMalloc((void **)&v, particle_count * sizeof (Vector));
	cudaMalloc((void **)&a, particle_count * sizeof (Vector));

	cudaMalloc((void **)&h, 	particle_count * sizeof (double));
	cudaMalloc((void **)&m, 	particle_count * sizeof (double));
	cudaMalloc((void **)&rho, particle_count * sizeof (double));
	
	//THERMAL
	cudaMalloc((void **)&k_T, particle_count * sizeof (Vector));
	cudaMalloc((void **)&cp_T, particle_count * sizeof (Vector));
		
	cudaMalloc((void **)&T		, particle_count * sizeof (double));
	cudaMalloc((void **)&dTdt	, particle_count * sizeof (double));


	
	// cudaMalloc((void**)&ppArray_a, 10 * sizeof(int*));
	// for(int i=0; i<10; i++) {
		// cudaMalloc(&someHostArray[i], 100*sizeof(int)); /* Replace 100 with the dimension that u want */
	// }

	
	//To allocate Neighbours, it is best to use a equal sized double array in order to be allocated once
}

// // Templatize data type, and host and device vars (of this type)
// template <typename T> copydata (const Domain &d, T *var_h, T *var_d){
	// T *var_h =  (Vector *)malloc(dom.Particles.size());
	// for (int i=0;i<dom.Particles.size();i++){
		// var_h[i] = dom.Particles[i]->T;
	// }
	// int size = dom.Particles.size() * sizeof(Vector);
	// cudaMemcpy(this->T, T, size, cudaMemcpyHostToDevice);
// }

//TEMPORARY, UNTIL EVERYTHING WILL BE CREATED ON DEVICE
void __host__ Domain_d::CopyData(const Domain& dom){
	
	//TODO TEMPLATIZE THIS!!
	double *T =  (double *)malloc(dom.Particles.size());
	for (int i=0;i<dom.Particles.size();i++){
		T[i] = dom.Particles[i]->T;
	}
	int size = dom.Particles.size() * sizeof(double);
	cudaMemcpy(this->T, T, size, cudaMemcpyHostToDevice);

	// for (int i=0;i<dom.Particles.size();i++){
		// T[i] = dom.Particles[i]->cp_T;
	// }
	// int size = dom.Particles.size() * sizeof(double);
	// cudaMemcpy(this->cp_T, T, size, cudaMemcpyHostToDevice);
	
}

//Thread per particle
//dTdt+=1/cp* (mass/dens^2)*4(k)
void __global__ ThermalSolveKernel (double *dTdt,
																		double3 *x, double *h,
																		double *m, double *rho,
																		double *T, double *k_T, double *cp, 
																		int **neib, int *neibcount){
	int i = threadIdx.x+blockDim.x*blockIdx.x;
	dTdt[i] = 0.;

	for (int k=0;k < neibcount[i];k++) { //Or size
		int j = neib[i][k];
		double3 xij; 
		xij = x[i] - x[j];
		double h_ = (h[i] + h[j])/2.0;
		double nxij = length(xij);
		
		double GK	= GradKernel(3, 0, nxij/h_, h_);
		//		Particles[i]->dTdt = 1./(Particles[i]->Density * Particles[i]->cp_T ) * ( temp[i] + Particles[i]->q_conv + Particles[i]->q_source);	
		//   mc[i]=mj/dj * 4. * ( P1->k_T * P2->k_T) / (P1->k_T + P2->k_T) * ( P1->T - P2->T) * dot( xij , v )/ (norm(xij)*norm(xij));
		dTdt[i] += m[j]/rho[j]*( 4.0*k_T[i]*k_T[j]/(k_T[i]+k_T[j]) * (T[i] - T[j])) * dot( xij , GK*xij )/(nxij*nxij);
	}
	dTdt[i] *=1/(rho[i]*cp[i]);
}

__global__ void TempCalcLeapfrogFirst(double *T, double *Ta, double *Tb, //output
																			double *dTdt, double dt){//input
	int i = threadIdx.x+blockDim.x*blockIdx.x;

	Ta[i] = T[i] - dt/2.0*dTdt[i];

}
__global__ void TempCalcLeapfrog     (double *T, double *Ta, double *Tb,
																		double *dTdt, double dt){
	int i = threadIdx.x+blockDim.x*blockIdx.x;
	
	Tb[i]  = Ta[i];
	Ta[i] += dTdt[i] * dt;
	T [i] = ( Ta[i] + Tb[i] ) / 2.;
}
//Originally in Particle::TempCalcLeapfrog
void Domain_d::ThermalSolve(){

	ThermalSolveKernel<<<1,1>>>(dTdt,	
																	x, h, //Vector has some problems
																	m, rho, 
																	T, k_T, cp_T,
																	neib, neibcount);
	
	if (isfirst_step) {
		TempCalcLeapfrogFirst<<< 1,1 >>>(T, Ta, Tb,
																		 dTdt, deltat);		
	} else {
		TempCalcLeapfrog <<< 1,1 >>>(T, Ta, Tb,
																		 dTdt, deltat);				
	}
}

Domain_d::~Domain_d(){
	
		cudaFree(a);		
		cudaFree(v);

		cudaFree(h);		
		cudaFree(m);
		cudaFree(rho);
		
}

};//SPH
    // // Create host pointer to array-like storage of device pointers
    // Obj** h_d_obj = (Obj**)malloc(sizeof(Obj*) * 3); //    <--------- SEE QUESTION 1
    // for (int i = 0; i < 3; i++) {
        // // Allocate space for an Obj and assign
        // cudaMalloc((void**)&h_d_obj[i], sizeof(Obj));
        // // Copy the object to the device (only has single scalar field to keep it simple)
        // cudaMemcpy(h_d_obj[i], &(h_obj[i]), sizeof(Obj), cudaMemcpyHostToDevice);
    // }

    // /**************************************************/
    // /* CREATE DEVICE ARRAY TO PASS POINTERS TO KERNEL */
    // /**************************************************/

    // // Create a pointer which will point to device memory
    // Obj** d_d_obj = NULL;
    // // Allocate space for 3 pointers on device at above location
    // cudaMalloc((void**)&d_d_obj, sizeof(Obj*) * 3);
    // // Copy the pointers from the host memory to the device array
    // cudaMemcpy(d_d_obj, h_d_obj, sizeof(Obj*) * 3, cudaMemcpyHostToDevice);
