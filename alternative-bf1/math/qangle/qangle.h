class QAngle;
class Vector;

class QAngle
{
	public:

		QAngle::QAngle ( void ) : x ( 0.0f ), y ( 0.0f ), z ( 0.0f )
		{

		}

		QAngle::QAngle ( float X, float Y, float Z )
		: x ( X ), y ( Y ), z ( Z )
		{

		}

		QAngle::QAngle ( float* p )
		{
			*this = p;
		};

		QAngle::QAngle ( QAngle& in )
		{
			*this = in;
		};

		QAngle::~QAngle ()
		{

		}

		inline QAngle& operator= ( const QAngle& in );
		inline QAngle& operator= ( float* p );
		inline QAngle& operator= ( float f );
		
		inline float& operator[] ( int i ) const;
		
		inline bool operator! ( void ) const;
		
		inline bool operator== ( const QAngle& other ) const;

		inline bool operator!= ( const QAngle& other ) const;

		inline QAngle& operator+= ( const QAngle& other );
		inline QAngle& operator+= ( float* p );
		inline QAngle& operator+= ( float f );

		inline QAngle& operator-= ( const QAngle& other );
		inline QAngle& operator-= ( float* p );
		inline QAngle& operator-= ( float f );
		
		inline QAngle& operator*= ( const QAngle& other );
		inline QAngle& operator*= ( float *p );
		inline QAngle& operator*= ( float f );
		
		inline QAngle& operator/= ( const QAngle& other );
		inline QAngle& operator/= ( float* p );
		inline QAngle& operator/= ( float f );
		
		inline QAngle operator+ ( const QAngle& other ) const;
		inline QAngle operator+ ( float* p ) const;
		inline QAngle operator+ ( float f ) const;
		
		inline QAngle operator- ( const QAngle& other ) const;
		inline QAngle operator- ( float* p ) const;
		inline QAngle operator- ( float f ) const;
		inline QAngle operator- ( void ) const;
		
		inline QAngle operator* ( const QAngle& other ) const;
		inline QAngle operator* ( float* p ) const;
		inline QAngle operator* ( float f ) const;
		
		inline QAngle operator/ ( const QAngle& other ) const;
		inline QAngle operator/ ( float* p ) const;
		inline QAngle operator/ ( float f ) const;

		operator float *()								{ return &x; }
		operator const float *() const					{ return &x; }

		inline bool IsZero ( void ) const
		{
			return x == 0.0f && y == 0.0f && z == 0.0f;
		}

		inline bool IsZero2D ( void )
		{
			return x == 0.0f && y == 0.0f;
		}

		inline QAngle& Clear ( void )
		{
			x = y = z = 0;

			return *this;
		}
		
		inline QAngle& Init ( float X, float Y, float Z )
		{
			x = X;
			y = Y;
			z = Z;

			return *this;
		}

		inline QAngle& Init ( float* p )
		{
			*this = p;

			return *this;
		}

		inline QAngle& Negate ( void )
		{
			x = -x;
			y = -y;
			z = -z;

			return *this;
		}
		
		QAngle& Normalize ( void )
		{
			float* Pointer = ( float* )this;

			for ( int i = 0; i < 3; i++ )
			{
				if ( Pointer[i] > 180 )
				{
					Pointer[i] -= 360;
				}
				else if ( Pointer[i] < -180 )
				{
					Pointer[i] += 360;
				}
			}

			return QAngle(Pointer[0], Pointer[1], Pointer[2]);
		}

		QAngle Delta360 ( QAngle& other )
		{
			QAngle OutDifference;
						 		
			OutDifference = other - *this;
 			
			for ( int i = 0; i < 3; i++ )
			{
				if ( OutDifference[i] < 0 )
				{
					OutDifference[i] = -OutDifference[i];
			
					if ( OutDifference[i] > 180 )
					{
						OutDifference[i] = 360 - OutDifference[i];
					}
				}
			}

			return OutDifference;
		}

		bool IsWithinFOV ( QAngle& AngIn, float fovX, float fovY )
		{
			QAngle AngleDifference = Delta360 ( AngIn );

			return AngleDifference.x <= fovX && AngleDifference.y <= fovY;
		}
				
		void AngleVectors ( Vector* Forward, Vector* Right, Vector* Up );
		void AngleVectorsTranspose ( Vector* Forward, Vector* Right, Vector* Up );

		inline bool IsValid()
		{
			return isfinite ( x ) && isfinite( y ) && isfinite( z );
		}

	public:

		float x, y, z;
};

inline QAngle& QAngle::operator= ( const QAngle& in )
{
	x = in.x;
	y = in.y;
	z = in.z;

	return *this;
}

inline QAngle& QAngle::operator= ( float* p )
{
	if ( p )
	{
		x = p[0]; y = p[1]; z = p[2];
	}
	else
	{
		x = y = z = 0;
	}

	return *this;
}

inline QAngle& QAngle::operator= ( float f )
{
	x = y = z = f;

	return *this;
}

inline float &QAngle::operator[] ( int i ) const
{
	if ( i >= 0 && i < 3 )
	{
		return ( ( float* )this )[i];
	}

	return ( ( float* )this )[0];
}

inline bool QAngle::operator! ( void ) const
{
	return IsZero();
}

inline bool QAngle::operator== ( const QAngle& other ) const
{
	return x == other.x && y == other.y && z == other.z;
}

inline bool QAngle::operator!= ( const QAngle& other ) const
{
	return x != other.x || y != other.y || z != other.z;
}

inline QAngle& QAngle::operator+= ( const QAngle& other )
{
	x += other.x;
	y += other.y;
	z += other.z;

	return *this;
}

inline QAngle& QAngle::operator+= ( float* p )
{
	x += p[0];
	y += p[1];
	z += p[2];

	return *this;
}

inline QAngle& QAngle::operator+= ( float f )
{
	x += f;
	y += f;
	z += f;

	return *this;
}

inline QAngle& QAngle::operator-= ( const QAngle& other )
{
	x -= other.x;
	y -= other.y;
	z -= other.z;

	return *this;
}

inline QAngle& QAngle::operator-= ( float* p )
{
	x -= p[0];
	y -= p[1];
	z -= p[2];

	return *this;
}
inline QAngle& QAngle::operator-= ( float f )
{
	x -= f;
	y -= f;
	z -= f;

	return *this;
}

inline QAngle& QAngle::operator*= ( const QAngle& other )
{
	x *= other.x;
	y *= other.y;
	z *= other.z;

	return *this;
}

inline QAngle& QAngle::operator*= ( float *p )
{
	x *= p[0];
	y *= p[1];
	z *= p[2];

	return *this;
}

inline QAngle& QAngle::operator*= ( float f )
{
	x *= f;
	y *= f;
	z *= f;

	return *this;
}

inline QAngle& QAngle::operator/= ( const QAngle& other )
{
	if ( other.x != 0 && other.y != 0 && other.z != 0 )
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
	}

	return *this;
}

inline QAngle& QAngle::operator/= ( float* p )
{
	if ( p[0] != 0 && p[1] != 0 && p[2] != 0 )
	{
		x /= p[0];
		y /= p[1];
		z /= p[2];
	}

	return *this;
}

inline QAngle& QAngle::operator/= ( float f )
{
	if ( f != 0 )
	{
		x /= f;
		y /= f;
		z /= f;
	}

	return *this;
}

inline QAngle QAngle::operator+ ( const QAngle& other ) const
{
	return QAngle ( x + other.x, y + other.y, z + other.z );
}

inline QAngle QAngle::operator+ ( float* p ) const
{
	return QAngle ( x + p[0], y + p[1], z + p[2] );
}

inline QAngle QAngle::operator+ ( float f ) const
{
	return QAngle ( x + f, y + f, z + f );
}

inline QAngle  QAngle::operator- ( const QAngle& other ) const
{
	return QAngle ( x - other.x, y - other.y, z - other.z );
}

inline QAngle  QAngle::operator- ( float* p ) const
{
	return QAngle ( x - p[0], y - p[1], z - p[2] );
}

inline QAngle  QAngle::operator- ( float f ) const
{
	return QAngle ( x - f, y - f, z - f );
}

inline QAngle QAngle::operator- ( void ) const
{
	return QAngle ( -x, -y, -z );
}

inline QAngle QAngle::operator* ( const QAngle& other ) const
{
	return QAngle ( x * other.x, y * other.y, z * other.z );
}

inline QAngle QAngle::operator* ( float* p ) const
{
	return QAngle ( x * p[0], y * p[1], z * p[2] );
}

inline QAngle QAngle::operator* ( float f ) const
{
	return QAngle ( x * f, y * f, z * f );
}

inline QAngle QAngle::operator/ ( const QAngle& other ) const
{
	if ( other.x != 0 && other.y != 0 && other.z != 0 )
	{
		return QAngle ( x / other.x, y / other.y, z / other.z );
	}

	return QAngle ( 0, 0, 0 );
}

inline QAngle QAngle::operator/ ( float* p ) const
{
	if ( p[0] != 0 && p[1] != 0 && p[2] != 0 )
	{
		return QAngle ( x / p[0], y / p[1], z / p[2] );
	}

	return QAngle ( 0, 0, 0 );
}

inline QAngle QAngle::operator/ ( float f ) const
{
	if ( f != 0 )
	{
		return QAngle ( x / f, y / f, z / f );
	}

	return QAngle ( 0, 0, 0 );
}

inline QAngle operator*(float fl, const QAngle& v)	{ return v * fl; };

