#include "../../includes.h"

void QAngle::AngleVectors ( Vector* Forward, Vector* Right, Vector* Up )
{
	float sp, sy, sr, cp, cy, cr, radx, rady, radz;

	radx = x * ( IM_PI*2 / 360 );
	rady = y * ( IM_PI*2 / 360 );
	radz = z * ( IM_PI*2 / 360 );

	sp = sin ( radx ); 
	sy = sin ( rady ); 
	sr = sin ( radz ); 

	cp = cos ( radx );
	cy = cos ( rady );
	cr = cos ( radz );

	if ( Forward )
	{
		Forward->x = cp * cy;
		Forward->y = cp * sy;
		Forward->z = -sp;
	}

	if ( Right )
	{
		Right->x = -1 * sr * sp * cy + -1 * cr * -sy;
		Right->y = -1 * sr * sp * sy + -1 * cr * cy;
		Right->z = -1 * sr * cp;
	}

	if ( Up )
	{
		Up->x = cr * sp * cy + -sr * -sy;
		Up->y = cr * sp * sy + -sr * cy;
		Up->z = cr * cp;
	}
}

void QAngle::AngleVectorsTranspose ( Vector* Forward, Vector* Right, Vector* Up )
{
	float sp, sy, sr, cp, cy, cr, radx, rady, radz;

	radx = x * ( IM_PI*2 / 360 );
	rady = y * ( IM_PI*2 / 360 );
	radz = z * ( IM_PI*2 / 360 );
		
	sp = sin ( radx ); 
	sy = sin ( rady ); 
	sr = sin ( radz ); 

	cp = cos ( radx );
	cy = cos ( rady );
	cr = cos ( radz );

	if ( Forward )
	{
		Forward->x = cp * cy;
		Forward->y = sr * sp * cy + cr * -sy;
		Forward->z = cr * sp * cy + -sr * -sy;
	}

	if ( Right )
	{
		Right->x = cp * sy;
		Right->y = sr * sp * sy + cr * cy;
		Right->z = cr * sp * sy + -sr * cy;
	}

	if ( Up )
	{
		Up->x = -sp;
		Up->y = sr * cp;
		Up->z = cr * cp;
	}
}