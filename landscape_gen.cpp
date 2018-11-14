#include <iostream>

using namespace std;

void squareStep(int array[], int step, int size, int *nextStep)
{
	*nextStep = size;
	for (int y = 0; y < (size - 1); y += step)
	{
		int y_max = y + step;
		int y_avg = (y + y + step);
		int y_mid = y_avg / 2;
		if (y_mid < *nextStep)
			*nextStep = y_mid;
		for (int x = 0; x < (size - 1); x += step)
		{
			int x_max = x + step;
			int x_avg = (x + x + step);
			int x_mid = x_avg / 2;
			int new_index = y_mid * size + x_mid;
			int index0 = y * size + x;
			int index1 = y * size + x_max;
			int index2 = y_max * size + x;
			int index3 = y_max * size + x_max;
			array[new_index] = array[index0] + array[index1] + array[index2] + array[index3];
		}
	}
}

void diamondStep(int array[], int step, int size)
{
	int true_step = 2 * step;
	for (int y = step; y < size; y += true_step)
	{
		int y_top = y - step * 2;
		int y_top_mid = y - step;
		int y_bottom = y + step * 2;
		int y_bottom_mid = y + step;
		bool top = y_top < 0;
		bool bottom = y_bottom > (size - 1);
		for (int x = step; x < size; x += true_step)
		{
			int x_left = x - step * 2;
			int x_left_mid = x - step;
			int x_right = x + step * 2;
			int x_right_mid = x + step;
			bool left = x_left < 0;
			bool right = x_right > (size - 1);

			// Center point
			int index0 = y * size + x;

			// Get New Left Point
			int new_index = y * size + x_left_mid;
			int index1 = y_top_mid * size + x_left_mid;
			int index2 = y_bottom_mid * size + x_left_mid;
			array[new_index] = array[index0] + array[index1] + array[index2];
			if (!left)
			{
				int index3 = y * size + x_left;
				array[new_index] += array[index3];
			}

			// Get New Top Point
			new_index = y_top_mid * size + x;
			index1 = y_top_mid * size + x_left_mid;
			index2 = y_top_mid * size + x_right_mid;
			array[new_index] = array[index0] + array[index1] + array[index2];
			if (!top)
			{
				int index3 = y_top * size + x;
				array[new_index] += array[index3];
			}

			if (right)
			{
				// Get New Right point if end of the line
				new_index = y * size + x_right_mid;
				index1 = y_top_mid * size + x_right_mid;
				index2 = y_bottom_mid * size + x_right_mid;
				array[new_index] = array[index0] + array[index1] + array[index2];
			}

			if (bottom)
			{
				// Get New Bottom Point if bottom of array
				new_index = y_bottom_mid * size + x;
				index1 = y_bottom_mid * size + x_left_mid;
				index2 = y_bottom_mid * size + x_right_mid;
				array[new_index] = array[index0] + array[index1] + array[index2];
			}
		}
	}
}

void printArray(int array[], int x_size, int y_size)
{
	for (int y = 0; y < y_size; y++)
	{
		for (int x = 0; x < x_size; x++)
		{
			int index = y * x_size + x;
			cout << array[index] << " ";
		}
		cout << endl;
	}
}

void generateMap(int array[], int size, int h0, int h1, int h2, int h3)
{
	int value = size - 1;

	// Check for invalid array size
	int n = 0;
	while (value != 1)
	{
		if (value % 2 != 0)
		{
			cout << "Invalid Array Size. Valid size is should be of the form '2^n - 1', where 'n' in a whole number." << endl;
			exit(1);
		}
		value = value / 2;
		n++;
	}
	int index0 = 0;
	int index1 = size - 1;
	int index2 = (size - 1) * size;
	int index3 = (size * size) - 1;
	array[index0] = h0;
	array[index1] = h1;
	array[index2] = h2;
	array[index3] = h3;
	int steps = n * 2;
	int step = size - 1;
	for (int i = 1; i <= steps; i++)
	{
		if (i % 2 == 0) {
			diamondStep(array, step, size);
		} else {
			squareStep(array, step, size, &step);
		}
	}
}

int main()
{
	int size = 25;
	int array[size];

	for (int i = 0; i < size; i++)
		array[i] = 0;

	generateMap(array, 5, 2, 3, 1, 2);
	printArray(array, 5, 5);
}