
//**************************************//
//										//
//			Simple Raytracer V1			//
//			A personal Project			//
//			By Matthew Walker			//
//				2/9/2020				//
//										//
//										//
//**************************************//


#define _USE_MATH_DEFINES
#define MAX_DEPTH 3

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <sstream>
#include <iomanip>


#include "Vec3f.h"
#include "Camera.h"
#include "Sphere.h"


Vec3f tracePixelColor(Vec3f &rayOrigin, Vec3f &rayDirection, Sphere &currentObject, float &closestPoint,
	std::vector<Sphere> &scene, std::vector<Vec3f> &lights, int &depth);
Vec3f calculateRayDirection(int columns, int rows, int x, int y, float fov, Camera camera);
std::ofstream writeToPPMFile(Vec3f** image, int width, int height, int maximumColorValue, int frameNum);



int main(int argc, char* argv[]) {

	bool isPerspective = true;

	//error checking
	if (argc < 2) {
		std::cout << "error: please specify a width, height, start frame, and end frame" << std::endl;
		return 0;

	}


	//Declare image variables
	const int columns = atoi(argv[1]);
	const int rows = atoi(argv[2]);
	const int startFrame = atoi(argv[3]);
	const int endFrame = atoi(argv[4]);
	int maximumColorValue = 255;

	//create a 2d grid of pixel objects
	//we initialize the ints in the pixels to 0 as a precaution
	Vec3f** image = new Vec3f * [rows];
	for (int i = 0; i < rows; i++) {
		image[i] = new Vec3f[columns];
	}

	for (int frameNum = startFrame; frameNum <= endFrame; frameNum++) {
		float currentTime = (float)frameNum / 24.0;


	//Declare objects
		Camera camera;
		camera.pos.setEach(0, 0, -15 + ((frameNum - 1.0) / 24.0) * 2);
		//camera.pos.setEach(0, 0, -15);

		std::vector<Vec3f> lights;

		
		lights.push_back(Vec3f(10, 5, 0));
		lights.push_back(Vec3f(-10, 5, 0));
		lights.push_back(Vec3f(5, 5, -5));
		lights.push_back(Vec3f(0, 5000, 0));
		

		std::vector<Sphere> scene;


		scene.push_back(Sphere(0, 0, 0, 0.7, 1, 1, 3, 1, 0, 1.1));

		scene.push_back(Sphere(4, -2.75, -2, 0.2, 0.9, 0.5, 1, 1, 0, 1.5));
		scene.push_back(Sphere(-5, -1.5, 1.5, 1, 0.4, 0.4, 2, 1, 0, 1.5));
		scene.push_back(Sphere(-3, -2.75, -2, 1, 1, 0, 1, 0, 0, 1.5));


		scene.push_back(Sphere(0, -10004, 0, 0.9, 0.9, 0.9, 10000));




		float aspectRatio = (float)columns / (float)rows;
		float fov = 50;

		int percentDone = 0;
		int percentIncThreshold = rows / 10;

		//For each pixel in the image
		for (int y = 0; y < rows; y++) {
			

			//output percentage of completion
			if (y > percentIncThreshold) {
				percentDone += 10;
				std::cout << "Frame " << frameNum << ": " << percentDone << "%" << std::endl;
				percentIncThreshold += (rows / 10);
			}

			//calculate light for each pixel
			for (int x = 0; x < columns; x++) {
				if (x == 250 && y == 250) {
				}
				image[y][x].setAll(0); //SET BACKGROUND COLOR
				float closestPoint = INFINITY;

				for (unsigned int i = 0; i < scene.size(); i++) {
					Vec3f rayOrigin(0);
					Vec3f rayDirection(0);

					if (isPerspective) { //use perspective projection

						rayOrigin.setEach(camera.pos.x, camera.pos.y, camera.pos.z);
						rayDirection = calculateRayDirection(columns, rows, x, y, fov, camera);

					}
					else { //use orthographic projection

						//The rayOrigin is the point on the pixel grid, but since the grid 
						//is centered at zero, we have to adjust the location of x and y
						rayOrigin.setEach(x - ((float)columns / 2), y - ((float)rows / 2), camera.pos.z);
						rayDirection.setEach(0, 0, 1);

					}
					
					int depth = 0;

					//call trace function
					image[y][x] = image[y][x] + tracePixelColor(rayOrigin, rayDirection, scene.at(i), 
						closestPoint, scene, lights, depth);
					if (image[y][x].x > 1) image[y][x].x = 1;
					if (image[y][x].y > 1) image[y][x].y = 1;
					if (image[y][x].z > 1) image[y][x].z = 1;

				}
			}
		}



		//Write image
		writeToPPMFile(image, rows, columns, maximumColorValue, frameNum);
	}



	return 0;
}


Vec3f tracePixelColor(Vec3f& rayOrigin, Vec3f& rayDirection, Sphere& currentObject, float& closestPoint,
	std::vector<Sphere>& scene, std::vector<Vec3f>& lights, int &depth) {
	
	int brightness = 155;
	Vec3f pixelColor = currentObject.color;

	float p0 = 0;
	float p1 = 1;

	//check for intersection
	if (currentObject.intersect(rayOrigin, rayDirection, p0, p1)) {

		float lightValue = 0;
		float gammaCorrect = 1.5;

		if (p0 < closestPoint) closestPoint = p0;
		else {
			//the sphere is obscured by a closer sphere
			return Vec3f(0);
			
		}

		//calculate spot where it intersected and the normal
		Vec3f intersectionPoint = rayOrigin + rayDirection * p0;
		Vec3f normal = intersectionPoint - currentObject.pos;
		normal.normalize();

		float epsilon = 1e-2;
		Vec3f normalOrigin = intersectionPoint + normal * epsilon;

		//calculate reflection and refraction
		if ((currentObject.refl > 0 || currentObject.refract > 0) && depth < MAX_DEPTH) {
			Vec3f reflDirection = rayDirection - normal * 2 * (rayDirection.dot(normal));
			reflDirection.normalize();


			depth += 1;
			
			//calculate reflection
			Vec3f reflColor(0);
			float newClosestPoint = INFINITY;
			for (int i = 0; i < scene.size(); i++) {
				reflColor = reflColor + tracePixelColor(normalOrigin, reflDirection, scene.at(i), newClosestPoint, scene, lights, depth);
			}

			//calculate refraction ***CURRENTLY HAVING ISSUES***
			float fresnel = 1;
			Vec3f refractColor(0);
			if (currentObject.refract > 0) {
				//eta represents the ratio of the two medium's IOR's. Air is ~1
				//so we just use IOR/1 (but we'll have to flip it when we travel from inside back out
				float eta = currentObject.IOR;
				float etaOutside = 1;
				float etaInside = currentObject.IOR;


				float angle = rayDirection.dot(normal);
				if (angle > 0) {
					normal = normal * -1;
					eta = 1 / currentObject.IOR;
					etaOutside = currentObject.IOR;
					etaInside = 1;
				}
				else angle *= -1;

				
				bool calculateFresnel = true;
				float critAngleCheck = 1 - eta * eta * (1 - angle * angle);
				if (critAngleCheck > 0) {
					Vec3f refractOrigin = intersectionPoint - normal * epsilon;
					Vec3f refractDir = (rayDirection * eta) + normal * ((eta * angle) - sqrt(critAngleCheck));
					refractDir.normalize();


					newClosestPoint = INFINITY;
					for (int i = 0; i < scene.size(); i++) {
						refractColor = refractColor + tracePixelColor(refractOrigin, refractDir, scene.at(i), newClosestPoint, scene, lights, depth);
					}

				}
				else {
					fresnel = 1; //total internal reflection
					calculateFresnel = false;
				}

				if (calculateFresnel) {
					//calculate fresnel
					float transmissionSinAngle = etaOutside / etaInside * sqrtf(1 - angle * angle));
					float angle2 = sqrtf(1 - transmissionSinAngle * transmissionSinAngle);
					float fr1 = ((etaInside * angle) - (etaOutside * angle2)) / ((etaInside * angle) + (etaOutside * angle2));
					float fr2 = ((etaOutside * angle) - (etaInside * angle2)) / ((etaOutside * angle) + (etaInside * angle2));

					fresnel = (fr1 * fr1 + fr2 * fr2) / 2;

				}
			}
			
			//Right now refraction will just default to 0
			pixelColor = pixelColor * ((reflColor * fresnel) + (refractColor * (1 - fresnel))); 
		}
		else {
			for (unsigned int k = 0; k < lights.size(); k++) {
				//shoot another ray towards the light and then take the dot product of the normal and the light
				Vec3f shadowRay = lights.at(k) - intersectionPoint;
				float lightDist = shadowRay.calculateMagnitude();
				shadowRay.normalize();
				float s0 = 0;
				float s1 = 0;
				bool inShadow = false;


				//check to see if another object is inbetween the light
				for (unsigned int j = 0; j < scene.size(); j++) {
					if (scene.at(j).intersect(normalOrigin, shadowRay, s0, s1) && s0 < lightDist) {
						inShadow = true;
						break;
					}
				}
				if (!inShadow) {
					lightValue = lightValue + normal.dot(shadowRay);
					if (lightValue < 0.02) lightValue = (float)0.02;


				}
			}
			lightValue = (lightValue / lights.size()) * gammaCorrect;
			pixelColor = pixelColor * lightValue;
		}

		return pixelColor;

	}
	
	return Vec3f(0);

}

Vec3f calculateRayDirection(int columns, int rows, int x, int y, float fov, Camera camera) {
	//so I have to convert the pixel grid into essentially an object in 3d space
	float NDCx = (x + 0.5) / columns;
	float NDCy = (y + 0.5) / rows;

	float aspectRatio = (float)columns / (float)rows;

	//these are the x and y coordinates of a grid that is 1 unit in front of the camera origin
	float xPixel = (2 * NDCx - 1) * aspectRatio * tan((fov * M_PI / 180) / 2);
	float yPixel = (1 - 2 * NDCy) * tan((fov * M_PI / 180) / 2);
	Vec3f pixelLocation(xPixel, yPixel, camera.pos.z + 1);

	//so now we set the ray origin and ray direction
	Vec3f rayOrigin(camera.pos.x, camera.pos.y, camera.pos.z);
	Vec3f rayDirection = pixelLocation - rayOrigin;


	rayDirection.normalize();
	return rayDirection;
}

std::ofstream writeToPPMFile(Vec3f** image, int rows, int columns, int maximumColorValue, int frameNum) {

	//Start creating the file
	std::ostringstream oss;
	char s[10];
	sprintf(s, "%04d", frameNum);
	oss << "./output/v4/image." << s << ".ppm";
	std::string outputLocation = oss.str();
	std::ofstream file(outputLocation, std::ios::binary);;

	file << "P6\n";
	file << columns << " " << rows << "\n";
	file << maximumColorValue << "\n";



	//for each pixel in the image
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {

			//each pixel has 3 values, R, G, and B
			//we have to convert the ints into chars because
			//otherwise the image isn't correct

			file <<
				(unsigned char)(image[i][j].x * 255) <<
				(unsigned char)(image[i][j].y * 255) <<
				(unsigned char)(image[i][j].z * 255);

		}
	}

	std::cout << "wrote output file to " << outputLocation << std::endl;




	file.close();

	return file;
}


