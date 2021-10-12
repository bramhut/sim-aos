﻿#include "sim-aos.h"

int main() {
	// CONTROL VARIABLES
	// uint64_t seed = 31728674;
	// const double size_enclosure = 1000000;
	// const double num_objects = 2;
	// const double time_step = 0.01;
	const double merge_distance = 500;

	// Check the input parameters
	const char *arguments[5] = {"num_objects", "num_iterations",
								"random_seed", "size_enclosure", "time_step"};
	std::cout << "sim-soa invoked with " << __argc - 1 << " parameters."
			  << "\n"
			  << "Arguments:\n";

	// Iterate for every argument needed
	for (int i = 1; i < 6; i++) {
		// Only assign variables that exist, variables that don't exist get an ?
		if (__argc > i) {
			std::cout << " " << arguments[i - 1] << ": " << __argv[i] << "\n";
		} else {
			std::cout << " " << arguments[i - 1] << ": ?"
					  << "\n";
		}
	}

	if (__argc != 6) {
		std::cerr << "Error: Wrong number of parameters";
		return -1;
	}

	const int num_objects = std::stoi(__argv[1]);
	const int num_iterations = std::stoi(__argv[2]);
	const uint64_t seed = std::stoull(__argv[3]);
	const double size_enclosure = std::stod(__argv[4]);
	const double time_step = std::stod(__argv[5]);

	if (num_objects < 0) {
		std::cerr << "Error: Invalid number of objects";
		return -1;
	}
	if (num_iterations < 0) {
		std::cerr << "Error: Invalid number of iterations";
		return -1;
	}
	if ((int)seed < 0) {
		std::cerr << "Error: Invalid seed";
		return -1;
	}
	if (size_enclosure < 0) {
		std::cerr << "Error: Invalid box size";
		return -1;
	}
	if (time_step < 0) {
		std::cerr << "Error: Invalid time increment";
		return -1;
	}

	// Initialize the RNG
	std::mt19937_64 gen(seed);
	std::uniform_real_distribution<> uniform_distr(0, size_enclosure);
	std::normal_distribution<double> normal_distr(1E21, 1E15);
	
	// Create the necessary amount of objects and store them in a vector of class Object
	std::vector<Object> objects(num_objects);
	auto rnd_object = [&gen, &uniform_distr, &normal_distr] {
		return Object(normal_distr(gen), uniform_distr(gen), uniform_distr(gen), uniform_distr(gen));
	};
	std::generate(objects.begin(), objects.end(), rnd_object);


	// Time loop
	for (size_t iteration = 0; iteration < num_iterations; iteration++) {

		// Reset all forces to zero
		for (auto& i : objects) {
			i.fx = i.fy = i.fz = 0;
		}

		// Caculate the force, change in velocity and position
		for (size_t i = 0; i < objects.size(); i++) {
			
			size_t objectsSize = objects.size();
			for (size_t j = i + 1; j < objectsSize; j++) {
				double massGravDist = objects[i].mass * objects[j].mass * G / dst_cube(objects[i], objects[j]);
				double fx = massGravDist * (objects[j].x - objects[i].x);
				double fy = massGravDist * (objects[j].y - objects[i].y);
				double fz = massGravDist * (objects[j].z - objects[i].z);
				objects[i].fx += fx;
				objects[j].fx -= fx;
				objects[i].fy += fy;
				objects[j].fy -= fy;
				objects[i].fz += fz;
				objects[j].fz -= fz;
			}

			// All forces on objects[i] are now computed, calculate the velocity change
			// F=ma -> a=F/m
			// dv=a*dt -> dv=F/m*dt
			objects[i].vx += objects[i].fx / objects[i].mass * time_step;
			objects[i].vy += objects[i].fy / objects[i].mass * time_step;
			objects[i].vz += objects[i].fz / objects[i].mass * time_step;

			// Update the position of the object
			objects[i].x += objects[i].vx * time_step;
			objects[i].y += objects[i].vy * time_step;
			objects[i].z += objects[i].vz * time_step;

			// Check for collisions (for all objects j < i)
			auto it = objects.begin();
			while (it != objects.begin() + i) { // for all objects j < i
				if (dst_sqr(objects[i], *it) < merge_distance) {
					// Collision detected, merge object j (it) into i
					objects[i].mass += (*it).mass;
					objects[i].vx += (*it).vx;
					objects[i].vy += (*it).vy;
					objects[i].vz += (*it).vz;
					it = objects.erase(it);
					i--; // Decrement i, as we just deleted a entry j < i
				}
				else {
					++it;
				}
			}
		}

		// Printing
		std::printf("it %d\t  x\t\t  y\t\t  z\n", (int)iteration);
		unsigned int j = 0;
		for (const auto& i : objects) {
			std::printf("%04d: f: %.2E \t%.2E \t%.2E\n", j, i.fx, i.fy, i.fz);
			std::printf("%04d: p: %.2E \t%.2E \t%.2E\n", j, i.x, i.y, i.z);
			std::printf("%04d: v: %.2E \t%.2E \t%.2E\n", j, i.vx, i.vy, i.vz);
			j++;
		}
		std::printf("Distance (0-1) %.2E\n", std::sqrt(dst_sqr(objects[0], objects[1])));
	}	// END OF TIME LOOP
	
	
	return 0;
}