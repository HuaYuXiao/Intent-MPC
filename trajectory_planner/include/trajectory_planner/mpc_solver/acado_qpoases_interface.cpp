/*
 *    This file was auto-generated using the ACADO Toolkit.
 *    
 *    While ACADO Toolkit is free software released under the terms of
 *    the GNU Lesser General Public License (LGPL), the generated code
 *    as such remains the property of the user who used ACADO Toolkit
 *    to generate this code. In particular, user dependent data of the code
 *    do not inherit the GNU LGPL license. On the other hand, parts of the
 *    generated code that are a direct copy of source code from the
 *    ACADO Toolkit or the software tools it is based on, remain, as derived
 *    work, automatically covered by the LGPL license.
 *    
 *    ACADO Toolkit is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *    
 */


extern "C"
{
#include "acado_common.h"
}

#include "INCLUDE/QProblem.hpp"

#if ACADO_COMPUTE_COVARIANCE_MATRIX == 1
#include "INCLUDE/EXTRAS/SolutionAnalysis.hpp"
#endif /* ACADO_COMPUTE_COVARIANCE_MATRIX */

static int acado_nWSR;



#if ACADO_COMPUTE_COVARIANCE_MATRIX == 1
static SolutionAnalysis acado_sa;
#endif /* ACADO_COMPUTE_COVARIANCE_MATRIX */
#include <iostream>
int acado_solve( void )
{
	acado_nWSR = QPOASES_NWSRMAX;

	QProblem qp(150, 1230);
	real_t cputime=0.08;
	memset(&acadoWorkspace.y,0,sizeof(acadoWorkspace.y));
	returnValue retVal = qp.init(acadoWorkspace.H, acadoWorkspace.g, acadoWorkspace.A, acadoWorkspace.lb, acadoWorkspace.ub, acadoWorkspace.lbA, acadoWorkspace.ubA, acado_nWSR,acadoWorkspace.y,&cputime);
    qp.getPrimalSolution( acadoWorkspace.x );
    qp.getDualSolution( acadoWorkspace.y );
	
#if ACADO_COMPUTE_COVARIANCE_MATRIX == 1

	if (retVal != SUCCESSFUL_RETURN)
		return (int)retVal;
		
	retVal = acado_sa.getHessianInverse( &qp,var );

#endif /* ACADO_COMPUTE_COVARIANCE_MATRIX */

	return (int)retVal;
}

int acado_getNWSR( void )
{
	return acado_nWSR;
}

const char* acado_getErrorString( int error )
{
	return MessageHandling::getErrorString( error );
}
