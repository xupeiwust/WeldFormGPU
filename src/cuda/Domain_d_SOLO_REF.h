/***********************************************************************************
* PersianSPH - A C++ library to simulate Mechanical Systems (solids, fluids        *
*             and soils) using Smoothed Particle Hydrodynamics method              *
* Copyright (C) 2013 Maziar Gholami Korzani and Sergio Galindo-Torres              *
*                                                                                  *
* This file is part of PersianSPH                                                  *
*                                                                                  *
* This is free software; you can redistribute it and/or modify it under the        *
* terms of the GNU General Public License as published by the Free Software        *
* Foundation; either version 3 of the License, or (at your option) any later       *
* version.                                                                         *
*                                                                                  *
* This program is distributed in the hope that it will be useful, but WITHOUT ANY  *
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A  *
* PARTICULAR PURPOSE. See the GNU General Public License for more details.         *
*                                                                                  *
* You should have received a copy of the GNU General Public License along with     *
* PersianSPH; if not, see <http://www.gnu.org/licenses/>                           *
************************************************************************************/

#ifndef SPH_Domain_d_CUH
#define SPH_Domain_d_CUH

// #include <stdio.h>    // for NULL
// #include <algorithm>  // for min,max


// #include <omp.h>

#include "Particle_d.cuh"
#include "Functions.h"
#include "tensor.cuh"
//#include "Boundary_Condition.h"

// //#ifdef _WIN32 /* __unix__ is usually defined by compilers targeting Unix systems */
// #include <sstream>
// //#endif
// #include <sstream>
// #include <string>
// #include <cmath>
// #include "tensor.h"

//C++ Enum used for easiness of coding in the input files
//enum Kernels_Type { Qubic_Spline=0, Quintic=1, Quintic_Spline=2 ,Hyperbolic_Spline=3};
//enum Viscosity_Eq_Type { Morris=0, Shao=1, Incompressible_Full=2, Takeda=3 };
//enum Gradient_Type { Squared_density=0, Multiplied_density=1 };

//#include <cuNSearch.h>



namespace SPH {

class Boundary;

class Domain_d
{
	
	//cuNSearch::NeighborhoodSearch neib;
	public:
	// typedef void (*PtVel) (Vec3_t & position, Vec3_t & Vel, double & Den, Boundary & bdry);
	// typedef void (*PtOut) (Particle * Particles, double & Prop1, double & Prop2,  double & Prop3);
	// typedef void (*PtDom) (Domain & dom);
    // // Constructor
    // Domain();

    // // Destructor
    // ~Domain();

    // // Domain Part
    // void AddSingleParticle	(int tag, Vec3_t const & x, double Mass, double Density, double h, bool Fixed);		//Add one particle
    // void AddBoxLength				(int tag, Vec3_t const &V, double Lx, double Ly, double Lz,double r, double Density,
																	// double h,int type, int rotation, bool random, bool Fixed);									//Add a cube of particles with a defined dimensions

	// void AddCylinderLength(int tag, Vec3_t const & V, double Rxy, double Lz, 
									// double r, double Density, double h, bool Fixed);

	// void AddTractionProbeLength(int tag, Vec3_t const & V, double Rxy, double Lz_side,
											// double Lz_neckmin,double Lz_necktot,double Rxy_center,
											// double r, double Density, double h, bool Fixed);
											
	// void Calculate3DMass(double Density);
	// void Add3DCubicBoxParticles(int tag, Vec3_t const & V, double Lx, double Ly, double Lz, 
									// double r, double Density, double h);


    // void AddBoxNo						(int tag, Vec3_t const &V, size_t nx, size_t ny, size_t nz,double r, double Density,
																	// double h,int type, int rotation, bool random, bool Fixed);									//Add a cube of particles with a defined numbers
    void DelParticles				(int const & Tags);					//Delete particles by tag
    // void CheckParticleLeave	();													//Check if any particles leave the domain, they will be deleted

    // void YZPlaneCellsNeighbourSearch(int q1);						//Create pairs of particles in cells of XZ plan
    // void MainNeighbourSearch				();									//Create pairs of particles in the whole domain
    // void StartAcceleration					(Vec3_t const & a = Vec3_t(0.0,0.0,0.0));	//Add a fixed acceleration such as the Gravity
	inline __device__ void StartAcceleration					();
	inline __device__ void PrimaryComputeAcceleration	();									//Compute the solid boundary properties
    inline __device__ void LastComputeAcceleration		();									//Compute the acceleration due to the other particles
    inline __device__ void CalcForce2233	(Particle * P1, Particle * P2);		//Calculates the contact force between soil-soil/solid-solid particles
    // void Move						(double dt);										//Move particles

    // void Solve					(double tf, double dt, double dtOut, char const * TheFileKey, size_t maxidx);		///< The solving function
    // void Solve_orig 			(double tf, double dt, double dtOut, char const * TheFileKey, size_t maxidx); 
	// void ThermalSolve			(double tf, double dt, double dtOut, char const * TheFileKey, size_t maxidx);		///< The solving function
	// void ThermalSolve_wo_init	(double tf, double dt, double dtOut, char const * TheFileKey, size_t maxidx);		///< The solving function


    // void Solve_wo_init (double tf, double dt, double dtOut, char const * TheFileKey, size_t maxidx);		///< The solving function	
	// //void Step(double tf, double dt, double dtOut, char const * TheFileKey, size_t maxidx);
	
    __device__ void CellInitiate		();															//Find the size of the domain as a cube, make cells and HOCs
    void ListGenerate		();															//Generate linked-list
    void CellReset			();															//Reset HOCs and particles' LL to initial value of -1
	
	// void ClearNbData();	
	
    // void WriteXDMF			(char const * FileKey);					//Save a XDMF file for the visualization


    // void InFlowBCLeave	();
    // void InFlowBCFresh	();
    inline __device__ void WholeVelocity	();

	// void Kernel_Set									(Kernels_Type const & KT);
	// void Viscosity_Eq_Set						(Viscosity_Eq_Type const & VQ);
	// void Gradient_Approach_Set			(Gradient_Type const & GT);
	
	// //Thermal Solver
	// void CalcTempInc 	(); 		//LUCIANO: Temperature increment
	// inline void CalcConvHeat ();
	// inline void CalcPlasticWorkHeat();
	// inline void CalcGradCorrMatrix();	//BONET GRADIENT CORRECTION

	
	
    // // Data
    Particle_d**				Particles; 	///< Array of particles
		int particlecount;					//
    // double					R;		///< Particle Radius in addrandombox

  double					sqrt_h_a;				//Coefficient for determining Time Step based on acceleration (can be defined by user)

    int 					Dimension;    	///< Dimension of the problem

    // double					MuMax;		///< Max Dynamic viscosity for calculating the timestep
    // double					CsMax;		///< Max speed of sound for calculating the timestep
	// double 					Vol;		///LUCIANO

    // Vec3_t					Gravity;       	///< Gravity acceleration

	// double 					hmax;		///< Max of h for the cell size  determination
	// Vec3_t                 			DomSize;	///< Each component of the vector is the domain size in that direction if periodic boundary condition is defined in that direction as well
	// double					rhomax;



    // int						*** HOC;	///< Array of "Head of Chain" for each cell

	// // BONET KERNEL CORRECTION
	// bool 					gradKernelCorr;	

	
    double 					XSPH;		///< Velocity correction factor
    // double 					InitialDist;	///< Initial distance of particles for Inflow BC

    // double					AvgVelocity;	///< Average velocity of the last two column for x periodic constant velocity
	double 					getCellfac(){return Cellfac;}

	// omp_lock_t 					dom_lock;	///< Open MP lock to lock Interactions array
  // Boundary					BC;
    // PtOut					UserOutput;
    // PtVel 					InCon;
    // PtVel 					OutCon;
    // PtVel 					AllCon;
    // Vec3_t					DomMax;
    // Vec3_t					DomMin;
    // PtDom					GeneralBefore;	///< Pointer to a function: to modify particles properties before CalcForce function
    // PtDom					GeneralAfter;	///< Pointer to a function: to modify particles properties after CalcForce function
    // size_t					Scheme;		///< Integration scheme: 0 = Modified Verlet, 1 = Leapfrog

  int**	SMPairs;
  int**	NSMPairs;
  int**	FSMPairs;
	
	int SMPairscount,NSMPairscount,FSMPairscount;
	
	
    int* 	FixedParticles;
    // Array< size_t >				FreeFSIParticles;
	  int FixedParticlescount;
	// double 	& getTime (){return Time;}		//LUCIANO

    // Array<std::pair<size_t,size_t> >		Initial;
    // Mat3_t I;
    // String					OutputName[3];
	// double T_inf;			//LUCIANO: IN CASE OF ONLY ONE CONVECTION TEMPERAURE
	
	// iKernel m_kernel;
	// bool					m_isNbDataCleared;
	// bool						auto_ts;				//LUCIANO: Auto Time Stepping
	
	
private:
		__device__ void Periodic_X_Correction	(float3 & x, double const & h, Particle * P1, Particle * P2);		//Corrects xij for the periodic boundary condition
		// void AdaptiveTimeStep				();		//Uses the minimum time step to smoothly vary the time step

		// void PrintInput			(char const * FileKey);		//Print out some initial parameters as a file
		// void InitialChecks	();		//Checks some parameter before proceeding to the solution
		// void TimestepCheck	();		//Checks the user time step with CFL approach

		// size_t					VisEq;					//Choose viscosity Eq based on different SPH discretisation
	int					KernelType;			//Choose a kernel
	int					GradientType;		//Choose a Gradient approach 1/Rho i^2 + 1/Rho j^2 or 1/(Rho i * Rho j)
	double 					Cellfac;				//Define the compact support of a kernel

	double					Time;    				//Current time of simulation at each solving step
	double					deltat;					//Time Step
    double					deltatmin;			//Minimum Time Step
    double					deltatint;			//Initial Time Step


};


/*inline*/ __host__ void StartAcceleration(Domain_d &sd); // This is the buffer function which calls the kernel
__global__ void StartAccelerationKernel(Domain_d &sd);

/*inline*/ __host__ void PrimaryComputeAcceleration(Domain_d &sd); // This is the buffer function which calls the kernel
__global__ void PrimaryComputeAccelerationKernel(Domain_d &sd);

/*inline*/ __host__ void LastComputeAcceleration(Domain_d &sd); // This is the buffer function which calls the kernel
__global__ void LastComputeAccelerationKernel(Domain_d &sd);

}; // namespace SPH

// #include "Interaction.cpp"
// #include "Domain.cpp"
// #include "Output.cpp"
// #include "InOutFlow.cpp"
// #include "Thermal.cpp"

#endif // SPH_DOMAIN_H