#include <bitmap.cpp>
#include <vector.cpp>
#include <algorithm>


char* getCmdOption(char**, char**, const std::string&);
bool cmdOptionExists(char**, char**, const std::string&);
//vec2f boxInter(vec3f, vec3f, vec3f, vec3f&);
float voxel(vec3f, vec3f);
vec3f render(vec3f, vec3f);
vec3f trayce(vec3f&, vec3f&);


struct Voxel {
	vec3f pos;
	vec3f rot;
	vec3f col;

	Voxel(vec3f p, vec3f r, vec3f c) {
		pos = p; rot = r; col = c;
	}
};


Bitmap *output;


const float pi = 3.14159265358979323846;


Voxel map[] = {
	Voxel(vec3f(0,0,4),vec3f(0),vec3f(1)),
	Voxel(vec3f(2,2,3),vec3f(0),vec3f(1,0,0)),
	Voxel(vec3f(-1,-1,3),vec3f(0),vec3f(1,1,0)),
};


float max_dist = 100;


int main(int argc, char** argv) {
	int width = 720;
	int height = 480;
	float fov_deg = 60;
	float fov;

	if (cmdOptionExists(argv, argv + argc, "-h")) {
		printf(
			"\n-h print this message\n"
			"-w output width\n"
			"-h output height\n"
			"-f viewing angle(degree)\n"
		);

		return 0;
	}

	if (cmdOptionExists(argv, argv + argc, "-w")) {
		char* width_str = getCmdOption(argv, argv + argc, "-w");
		width = atoi(width_str);
	}

	if (cmdOptionExists(argv, argv + argc, "-h")) {
		char* height_str = getCmdOption(argv, argv + argc, "-h");
		height = atoi(height_str);
	}

	if (cmdOptionExists(argv, argv + argc, "-f")) {
		char* fov_str = getCmdOption(argv, argv + argc, "-f");
		fov_deg = atoi(fov_str);
	}

	fov = (fov_deg - 30) / 180. * pi;

	printf("width - %i\nheight - %i\nfov(degree) - %f", width, height, fov_deg);

	output = new Bitmap(width, height);

	
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			float u = (float)x / width  * 2. - 1.;
			float v = (float)y / height * 2. - 1.;
			u *= (float)width / height;

			vec3f ro = vec3f(0,0,0);
			vec3f rd = normalize(vec3f(u,v,cos(fov)));

			vec3f color = render(ro, rd);

			output->set(x, y, color.x * 255, color.y * 255, color.z * 255);
		}
	}

	output->save("output.bmp");

	return 0;
}



vec3f render(vec3f ro, vec3f rd) {
	return trayce(ro, rd);
}


vec3f trayce(vec3f& ro, vec3f& rd) {
	float min_inter = max_dist;
	vec3f color = vec3f(0);

	for (int i = 0; i < sizeof(map) / sizeof(Voxel); i++) {
		float inter = voxel(ro - map[i].pos, rd);

		if (inter > 0. & inter < min_inter) {
			min_inter = inter;
			color = map[i].col;
		}
	}
	
	return color;
}


vec2f boxInter(vec3f ro, vec3f rd, vec3f rad, vec3f& oN)  {
	vec3f m = vec3f(1.0) / rd;
	vec3f n = m * ro;
	vec3f k = abs(m) * rad;
	vec3f t1 = -n - k;
	vec3f t2 = -n + k;
	float tN = fmax(fmax(t1.x, t1.y), t1.z);
	float tF = fmin(fmin(t2.x, t2.y), t2.z);
	if(tN > tF || tF < 0.0) return vec2f(-1);
	//oN = -sign(rd) * step(t1.yzx, t1.xyz) * step(t1.zxy, t1.xyz);
	vec3f a(t1.y, t1.z, t1.x);
	vec3f b(t1.z, t1.x, t1.y);
	oN = -sign(rd) * step(a, t1) * step(b, t1);
	return vec2f(tN, tF);
}

float voxel(vec3f ro, vec3f rd) {
	vec3f n;
	return boxInter(ro, rd, vec3f(0.5), n).x;
}


// https://stackoverflow.com/questions/865668/parsing-command-line-arguments-in-c
char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}