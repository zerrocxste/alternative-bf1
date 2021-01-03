class QAngle;
class Vector;

extern Vector Rotate(Vector& src, float rotate);

class Vector
{
public:

	Vector::Vector(void) : x(0.0f), y(0.0f), z(0.0f)
	{

	}

	Vector::Vector(float X, float Y, float Z)
		: x(X), y(Y), z(Z)
	{

	}

	Vector::Vector(float* p)
	{
		*this = p;
	};

	Vector::Vector(const Vector& in)
	{
		x = in.x;
		y = in.y;
		z = in.z;
	};

	Vector::~Vector()
	{

	}


	/*inline Vector(float XYZ)
		: x(XYZ), y(XYZ), z(XYZ)
	{
	}*/

	inline Vector& operator= (const Vector& in);
	inline Vector& operator= (float* p);
	inline Vector& operator= (float f);

	inline float& operator[] (int i) const;

	inline bool operator! (void) const;

	inline bool operator== (const Vector& other) const;

	inline bool operator!= (const Vector& other) const;

	inline Vector& operator+= (const Vector& other);
	inline Vector& operator+= (float* p);
	inline Vector& operator+= (float f);

	inline Vector& operator-= (const Vector& other);
	inline Vector& operator-= (float* p);
	inline Vector& operator-= (float f);

	inline Vector& operator*= (const Vector& other);
	inline Vector& operator*= (float *p);
	inline Vector& operator*= (float f);

	inline Vector& operator/= (const Vector& other);
	inline Vector& operator/= (float* p);
	inline Vector& operator/= (float f);

	inline Vector operator+ (const Vector& other) const;
	inline Vector operator+ (float* p) const;
	inline Vector operator+ (float f) const;

	inline Vector operator- (const Vector& other) const;
	inline Vector operator- (float* p) const;
	inline Vector operator- (float f) const;
	inline Vector operator- (void) const;

	inline Vector operator* (const Vector& other) const;
	inline Vector operator* (float* p) const;
	inline Vector operator* (float f) const;

	inline Vector operator/ (const Vector& other) const;
	inline Vector operator/ (float* p) const;
	inline Vector operator/ (float f) const;

	operator float *() { return &x; }
	operator const float *() const { return &x; }

	inline bool Vector::IsZero(void) const
	{
		return x == 0.0f && y == 0.0f && z == 0.0f;
	}

	inline bool Vector::IsZero2D(void)
	{
		return x == 0.0f && y == 0.0f;
	}

	inline Vector& Clear(void)
	{
		x = y = z = 0;

		return *this;
	}

	inline Vector& Init(float X, float Y, float Z)
	{
		x = X;
		y = Y;
		z = Z;

		return *this;
	}

	inline Vector& Init(float* p)
	{
		*this = p;

		return *this;
	}

	inline Vector& Negate(void)
	{
		x = -x;
		y = -y;
		z = -z;

		return *this;
	}

	inline float Dot(const Vector& other)
	{
		return x * other.x + y * other.y + z * other.z;
	}

	inline float Dot2D(const Vector& other)
	{
		return x * other.x + y * other.y;
	}

	inline float Length(void)
	{
		float Length = 0;

		Length = sqrt(LengthSqr());

		return Length;
	}

	inline float Length2D(void)
	{
		float Length = 0;

		Length = sqrt(LengthSqr2D());

		return Length;
	}

	inline float LengthSqr(void)
	{
		return x * x + y * y + z * z;
	}

	inline float LengthSqr2D(void)
	{
		return x * x + y * y;
	}

	inline float Distance(Vector& ToVector)
	{
		return (ToVector - *this).Length();
	}

	inline float Distance2D(Vector& ToVector)
	{
		return (ToVector - *this).Length2D();
	}

	inline float Normalize(void)
	{
		float RecipLength, VectorLength;

		VectorLength = Length();

		if (VectorLength != 0)
		{
			RecipLength = 1 / VectorLength;

			x *= RecipLength;
			y *= RecipLength;
			z *= RecipLength;
		}

		return VectorLength;
	}

	inline Vector Normalized(void)
	{
		float flLen = Length();
		if (flLen == 0) return Vector(0, 0, 1);
		flLen = 1 / flLen;
		return Vector(x * flLen, y * flLen, z * flLen);
	}

	inline float Normalize2D(void)
	{
		float Length = LengthSqr2D();
		float RecipLength;

		if (Length != 0)
		{
			RecipLength = 1 / Length;

			x *= RecipLength;
			y *= RecipLength;
		}

		return Length;
	}

#define IM_PII           3.14159265358979323846f

	inline float AngleBetween(Vector& other)
	{
		other.Normalize();
		Normalize();

		return acos(Dot(other)) * static_cast<float>(180.0 / IM_PII);
	}

	Vector &CrossProduct(const Vector& a, const Vector& b)
	{
		x = (a.y * b.z) - (a.z * b.y);
		y = (a.z * b.x) - (a.x * b.z);
		z = (a.x * b.y) - (a.y * b.x);

		return *this;
	}
	QAngle ToEulerAngles();
	QAngle ToEulerAngles(Vector* PseudoUp);
	void AngleMatrix(QAngle& Rotation, float(*matrix)[3]);
	void VectorRotate(Vector& In, QAngle& Rotation);

	QAngle ClampAngle(QAngle angle);

	void VectorVectors(Vector& Right, Vector& Up)
	{
		Vector tmp;

		if (x == 0 && y == 0)
		{
			Right[0] = 1;
			Right[1] = 0;
			Right[2] = 0;
			Up[0] = -z;
			Up[1] = 0;
			Up[2] = 0;
			return;
		}

		tmp[0] = 0; tmp[1] = 0; tmp[2] = 1.0;
		CrossProduct(tmp, Right);
		Right.Normalize();
		Right.CrossProduct(*this, Up);
		Up.Normalize();
	}

	inline bool IsValid()
	{
		return isfinite(x) && isfinite(y) && isfinite(z);
	}

public:

	float x, y, z;
};

inline Vector& Vector::operator= (const Vector& in)
{
	x = in.x;
	y = in.y;
	z = in.z;

	return *this;
}

inline Vector& Vector::operator= (float* p)
{
	if (p)
	{
		x = p[0]; y = p[1]; z = p[2];
	}
	else
	{
		x = y = z = 0;
	}

	return *this;
}

inline Vector& Vector::operator= (float f)
{
	x = y = z = f;

	return *this;
}

inline float &Vector::operator[] (int i) const
{
	if (i >= 0 && i < 3)
	{
		return ((float*)this)[i];
	}

	return ((float*)this)[0];
}

inline bool Vector::operator! (void) const
{
	return IsZero();
}

inline bool Vector::operator== (const Vector& other) const
{
	return x == other.x && y == other.y && z == other.z;
}

inline bool Vector::operator!= (const Vector& other) const
{
	return x != other.x || y != other.y || z != other.z;
}

inline Vector& Vector::operator+= (const Vector& other)
{
	x += other.x;
	y += other.y;
	z += other.z;

	return *this;
}

inline Vector& Vector::operator+= (float* p)
{
	x += p[0];
	y += p[1];
	z += p[2];

	return *this;
}

inline Vector& Vector::operator+= (float f)
{
	x += f;
	y += f;
	z += f;

	return *this;
}

inline Vector& Vector::operator-= (const Vector& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;

	return *this;
}

inline Vector& Vector::operator-= (float* p)
{
	x -= p[0];
	y -= p[1];
	z -= p[2];

	return *this;
}
inline Vector& Vector::operator-= (float f)
{
	x -= f;
	y -= f;
	z -= f;

	return *this;
}

inline Vector& Vector::operator*= (const Vector& other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;

	return *this;
}

inline Vector& Vector::operator*= (float *p)
{
	x *= p[0];
	y *= p[1];
	z *= p[2];

	return *this;
}

inline Vector& Vector::operator*= (float f)
{
	x *= f;
	y *= f;
	z *= f;

	return *this;
}

inline Vector& Vector::operator/= (const Vector& other)
{
	if (other.x != 0 && other.y != 0 && other.z != 0)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
	}

	return *this;
}

inline Vector& Vector::operator/= (float* p)
{
	if (p[0] != 0 && p[1] != 0 && p[2] != 0)
	{
		x /= p[0];
		y /= p[1];
		z /= p[2];
	}

	return *this;
}

inline Vector& Vector::operator/= (float f)
{
	if (f != 0)
	{
		x /= f;
		y /= f;
		z /= f;
	}

	return *this;
}

inline Vector Vector::operator+ (const Vector& other) const
{
	return Vector(x + other.x, y + other.y, z + other.z);
}

inline Vector Vector::operator+ (float* p) const
{
	return Vector(x + p[0], y + p[1], z + p[2]);
}

inline Vector Vector::operator+ (float f) const
{
	return Vector(x + f, y + f, z + f);
}

inline Vector  Vector::operator- (const Vector& other) const
{
	return Vector(x - other.x, y - other.y, z - other.z);
}

inline Vector  Vector::operator- (float* p) const
{
	return Vector(x - p[0], y - p[1], z - p[2]);
}

inline Vector  Vector::operator- (float f) const
{
	return Vector(x - f, y - f, z - f);
}

inline Vector Vector::operator- (void) const
{
	return Vector(-x, -y, -z);
}

inline Vector Vector::operator* (const Vector& other) const
{
	return Vector(x * other.x, y * other.y, z * other.z);
}

inline Vector Vector::operator* (float* p) const
{
	return Vector(x * p[0], y * p[1], z * p[2]);
}

inline Vector Vector::operator* (float f) const
{
	return Vector(x * f, y * f, z * f);
}

inline Vector Vector::operator/ (const Vector& other) const
{
	if (other.x != 0 && other.y != 0 && other.z != 0)
	{
		return Vector(x / other.x, y / other.y, z / other.z);
	}

	return *this;
}

inline Vector Vector::operator/ (float* p) const
{
	if (p[0] != 0 && p[1] != 0 && p[2] != 0)
	{
		return Vector(x / p[0], y / p[1], z / p[2]);
	}

	return *this;
}

inline Vector Vector::operator/ (float f) const
{
	if (f != 0)
	{
		return Vector(x / f, y / f, z / f);
	}

	return *this;
}

inline Vector operator*(float fl, const Vector& v) { return v * fl; };

inline float DotProduct(const Vector& a, const Vector& b) { return(a.x*b.x + a.y*b.y + a.z*b.z); }

#define vec3_t Vector

class Vector4 
{
public:
	Vector4::Vector4(void) : x(0.f), y(0.f), z(0.f), w(0.f) {}
	Vector4::Vector4(float X, float Y, float Z, float W)
		: x(X), y(Y), z(Z), w(W)
	{

	}
	float x, y, z, w;
};