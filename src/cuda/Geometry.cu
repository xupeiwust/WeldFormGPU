#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "Domain_d.cuh"
#include "Functions.cuh"
#include "tensor.cuh"
#include "tensor3.cu" //INLINE

#include <iostream>
  
namespace SPH {
  
__global__ inline void CalculateSurfaceKernel(Domain_d *dom_d,	const uint *particlenbcount,
																	const uint *neighborWriteOffsets,
																	const uint *neighbors,
																	/*const int &id, */const double &totmass){
	dom_d->CalculateSurface(
	particlenbcount,
	neighborWriteOffsets,
	neighbors,
	/*id, */dom_d->totmass);

}
                                  
// Calculate Free Surface (for contact and heat convection)
void __device__ inline Domain_d::CalculateSurface(const uint *particlenbcount,
                                                  const uint *neighborWriteOffsets,
                                                  const uint *neighbors,
                                                  /*const int &id, */const double &totmass){
	//id_free_surf = id;

	int i = threadIdx.x + blockDim.x*blockIdx.x;
	
	if ( i < first_fem_particle_idx[0] ) { //In Contact Surface Particles, normal are updated different way
    normal[i] = make_double3(0.,0.,0.);

    int neibcount = particlenbcount[i];
    int nbcount_corr = 0; //WITHOUT CONTACT SURFACE!!
    const uint writeOffset = neighborWriteOffsets[i];
    
    
      for (int k=0;k < neibcount; k++) { //Or size
        //if fixed size i = part * NB + k
        //int j = neib[i][k];
        int j = neighbors[writeOffset + k];
        //double h	= partdata->h[i]+P2->h)/2;
        double3 xij = x[i] - x[j];
        for (int mc=0;mc<trimesh_count;mc++){
          if (ID[j]!=contact_surf_id[mc]){  //EXCLUDE RIGID PAIRS!
            normal[i] += m[j] * xij; 
          // if (i==0)
          //printf("particle %d Nb %d xij: %f %f %f mj %.6e\n", i, j, xij.x, xij.y, xij.z, m[j]);
          nbcount_corr++;
          }
        }
      }//

      // if (i==0)
        // printf("particle %d normal : %f %f %f , nb %d totmass %f\n", i, normal[i].x, normal[i].y, normal[i].z, nbcount_corr, totmass);
      
      normal[i]*= ((double)particle_count/(totmass *(double)nbcount_corr)); //Attention parenthesis, if not it crashes
      //normal[i]*= 1./totmass;
      // if (i==0)
        // printf("particle %d normal : %f %f %f , nb %d length %f\n", i, normal[i].x, normal[i].y, normal[i].z, nbcount_corr, length (normal[i]));
      if ( length(normal[i]) >= 0.25 * h[i] && nbcount_corr <= 46) {//3-114 Fraser {
        if (!not_write_surf_ID[i])
          ID[i] = id_free_surf; //THIS CRASH IS ASSIGNED BY PARAMETER
        //printf("particle %d normal : %f %f %f , nb %d\n", i, normal[i].x, normal[i].y, normal[i].z, nbcount_corr);
        // //surf_part++;
      }
  
  }//i < particle_count
  
}

}; //SPH

#endif