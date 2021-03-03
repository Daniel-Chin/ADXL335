#include "ADXL335.h"

class CircumcentreSolver {
	// Modified from https://stackoverflow.com/a/13601950/8622053
	// Original author: June Rhodes, 
	/// Given four points in 3D space, solves for a sphere such that all four points
	/// lie on the sphere's surface.
	/// Translated from Javascript on http://www.convertalot.com/sphere_solver.html, originally
	float ZERO = 0;
	double m_X0, m_Y0, m_Z0;
	double m_Radius;
	double P[4][3] = {
		{ ZERO, ZERO, ZERO },
		{ ZERO, ZERO, ZERO },
		{ ZERO, ZERO, ZERO },
		{ ZERO, ZERO, ZERO }
	};

	/// <summary>
	/// Whether the result was a valid sphere.
	/// </summary>
	bool isValid() {
		return this->m_Radius != 0;
	}

	/// <summary>
	/// Computes the centre of a sphere such that all four specified points in
	/// 3D space lie on the sphere's surface.
	/// </summary>
	/// <param name="a">The first point (array of 3 doubles for X, Y, Z).</param>
	/// <param name="b">The second point (array of 3 doubles for X, Y, Z).</param>
	/// <param name="c">The third point (array of 3 doubles for X, Y, Z).</param>
	/// <param name="d">The fourth point (array of 3 doubles for X, Y, Z).</param>
	CircumcentreSolver(double a[], double b[], double c[], double d[])
	{
		P[0][0] = a[0];
		P[0][1] = a[1];
		P[0][2] = a[2];
		P[1][0] = b[0];
		P[1][1] = b[1];
		P[1][2] = b[2];
		P[2][0] = c[0];
		P[2][1] = c[1];
		P[2][2] = c[2];
		P[3][0] = d[0];
		P[3][1] = d[1];
		P[3][2] = d[2];

		// Compute result sphere.
		this->Sphere();
	}

	void Sphere() {
		double r, m11, m12, m13, m14, m15;
		double a[4][4] = {
			{ ZERO, ZERO, ZERO, ZERO },
			{ ZERO, ZERO, ZERO, ZERO },
			{ ZERO, ZERO, ZERO, ZERO },
			{ ZERO, ZERO, ZERO, ZERO }
		};

		// Find minor 1, 1.
		for (int i = 0; i < 4; i++)
		{
			a[i][0] = P[i][0];
			a[i][1] = P[i][1];
			a[i][2] = P[i][2];
			a[i][3] = 1;
		}
		m11 = this->Determinant(a, 4);

		// Find minor 1, 2.
		for (int i = 0; i < 4; i++)
		{
			a[i][0] = P[i][0] * P[i][0] + P[i][1] * P[i][1] + P[i][2] * P[i][2];
			a[i][1] = P[i][1];
			a[i][2] = P[i][2];
			a[i][3] = 1;
		}
		m12 = this->Determinant(a, 4);

		// Find minor 1, 3.
		for (int i = 0; i < 4; i++)
		{
			a[i][0] = P[i][0] * P[i][0] + P[i][1] * P[i][1] + P[i][2] * P[i][2];
			a[i][1] = P[i][0];
			a[i][2] = P[i][2];
			a[i][3] = 1;
		}
		m13 = this->Determinant(a, 4);

		// Find minor 1, 4.
		for (int i = 0; i < 4; i++)
		{
			a[i][0] = P[i][0] * P[i][0] + P[i][1] * P[i][1] + P[i][2] * P[i][2];
			a[i][1] = P[i][0];
			a[i][2] = P[i][1];
			a[i][3] = 1;
		}
		m14 = this->Determinant(a, 4);

		// Find minor 1, 5.
		for (int i = 0; i < 4; i++)
		{
			a[i][0] = P[i][0] * P[i][0] + P[i][1] * P[i][1] + P[i][2] * P[i][2];
			a[i][1] = P[i][0];
			a[i][2] = P[i][1];
			a[i][3] = P[i][2];
		}
		m15 = this->Determinant(a, 4);

		// Calculate result.
		if (m11 == 0) {
			this->m_X0 = 0;
			this->m_Y0 = 0;
			this->m_Z0 = 0;
			this->m_Radius = 0;
		} else {
			this->m_X0 = 0.5 * m12 / m11;
			this->m_Y0 = -0.5 * m13 / m11;
			this->m_Z0 = 0.5 * m14 / m11;
			this->m_Radius = sqrt(this->m_X0 * this->m_X0 + this->m_Y0 * this->m_Y0 + this->m_Z0 * this->m_Z0 - m15 / m11);
		}
	}

	/// <summary>
	/// Recursive definition of determinate using expansion by minors.
	/// </summary>
	double Determinant(double a[4][4], int n)
	{
			int i, j, j1, j2;
			double d = 0;
			double m[4][4] = 
							{
									{ ZERO, ZERO, ZERO, ZERO },
									{ ZERO, ZERO, ZERO, ZERO },
									{ ZERO, ZERO, ZERO, ZERO },
									{ ZERO, ZERO, ZERO, ZERO }
							};

			if (n == 2)
			{
					// Terminate recursion.
					d = a[0][0] * a[1][1] - a[1][0] * a[0][1];
			}
			else
			{
					d = 0;
					for (j1 = 0; j1 < n; j1++) // Do each column.
					{
							for (i = 1; i < n; i++) // Create minor.
							{
									j2 = 0;
									for (j = 0; j < n; j++)
									{
											if (j == j1) continue;
											m[i - 1][j2] = a[i][j];
											j2++;
									}
							}

							// Sum (+/-)cofactor * minor.
							d = d + pow(-1.0, j1) * a[0][j1] * this->Determinant(m, n - 1);
					}
			}

			return d;
	}
};

int zero_x;
int zero_y;
int zero_z;
float sensitivity;
ADXL335 accelerometer;
CircumcentreSolver solver;

void setup() {
	Serial.begin(9600);
	accelerometer.begin();
	int x, y, z;
	for(int i = 0; i < 20; i ++) {
		accelerometer.getXYZ(&x,&y,&z);
	}
	calibrate();
}

void loop() {}

void pause() {
	Serial.println("please type any charactor if you are ready... ");
	while(Serial.available() == 0);
	delay(100);
	while(Serial.available() > 0) Serial.read();
}

void calibrate() {
	Serial.println("The calibration starts. ");
	Serial.println("Follow instructions at . ");
	pause();

	int x, y, z;
	double[] a = new double[3];
	double[] b = new double[3];
	double[] c = new double[3];
	double[] d = new double[3];

	Serial.println("Z up.");
	pause();
	accelerometer.getXYZ(&a[0], &a[1], &a[2]);

	Serial.println("X up.");
	pause();
	accelerometer.getXYZ(&b[0], &b[1], &b[2]);

	Serial.println("Y up.");
	pause();
	accelerometer.getXYZ(&c[0], &c[1], &c[2]);

	Serial.println("Z down.");
	pause();
	accelerometer.getXYZ(&d[0], &d[1], &d[2]);

	solver = new CircumcentreSolver(a, b, c, d);

	float zero_xv, zero_yv, zero_zv;
	zero_xv = (float)solver.m_X0 * ADC_REF / ADC_AMPLITUDE;
	zero_yv = (float)solver.m_Y0 * ADC_REF / ADC_AMPLITUDE;
	zero_zv = (float)solver.m_Z0 * ADC_REF / ADC_AMPLITUDE;
	sensitivity = (float)solver.m_Radius * ADC_REF / ADC_AMPLITUDE;
	Serial.println("please modify the macro definitions with the below in ADXL335.h");
	Serial.print("  ZERO_X = ");
	Serial.println(zero_xv);
	Serial.print("  ZERO_Y = ");
	Serial.println(zero_yv);
	Serial.print("  ZERO_Z = ");
	Serial.println(zero_zv);
	Serial.print("  SENSITIVITY = ");
	Serial.println(sensitivity, 2);
	Serial.println("please modify the macro definitions with the above in ADXL335.h");
}
