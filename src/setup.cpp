#include "setup.hpp"
#include "fluiddata.hpp"
#include "graphics.hpp"
#include <iostream>
#include <fstream>
#include <random>
#include <filesystem>

#ifdef BENCHMARK
#include "info.hpp"
void main_setup() { // benchmark; required extensions in defines.hpp: BENCHMARK, optionally FP16S or FP16C
	// ################################################################## define simulation box size, viscosity and volume force ###################################################################
	uint mlups = 0u; {

		//LBM lbm( 32u,  32u,  32u, 1.0f);
		//LBM lbm( 64u,  64u,  64u, 1.0f);
		//LBM lbm(128u, 128u, 128u, 1.0f);
		LBM lbm(256u, 256u, 256u, 1.0f); // default
		//LBM lbm(384u, 384u, 384u, 1.0f);
		//LBM lbm(512u, 512u, 512u, 1.0f);

		//const uint memory = 1488u; // memory occupation in MB (for multi-GPU benchmarks: make this close to as large as the GPU's VRAM capacity)
		//const uint3 lbm_N = resolution(float3(1.0f, 1.0f, 1.0f), memory); // input: simulation box aspect ratio and VRAM occupation in MB, output: grid resolution
		//LBM lbm(1u*lbm_N.x, 1u*lbm_N.y, 1u*lbm_N.z, 1u, 1u, 1u, 1.0f); // 1 GPU
		//LBM lbm(2u*lbm_N.x, 1u*lbm_N.y, 1u*lbm_N.z, 2u, 1u, 1u, 1.0f); // 2 GPUs
		//LBM lbm(2u*lbm_N.x, 2u*lbm_N.y, 1u*lbm_N.z, 2u, 2u, 1u, 1.0f); // 4 GPUs
		//LBM lbm(2u*lbm_N.x, 2u*lbm_N.y, 2u*lbm_N.z, 2u, 2u, 2u, 1.0f); // 8 GPUs

		// #########################################################################################################################################################################################
		for(uint i=0u; i<1000u; i++) {
			lbm.run(10u, 1000u*10u);
			mlups = max(mlups, to_uint((double)lbm.get_N()*1E-6/info.runtime_lbm_timestep_smooth));
		}
	} // make lbm object go out of scope to free its memory
	print_info("Peak MLUPs/s = "+to_string(mlups));
#if defined(_WIN32)
	wait();
#endif // Windows
} /**/
#endif // BENCHMARK


void main_setup() { // without decay
	std::string filename = PROPERTY_PATH;
	std::vector<Material> materials = readCSV(filename, VISC_RANGE);
	if (materials.empty()) {
		std::cerr << "No data found!" << std::endl;
		return;
	}
#if defined(INTERACTIVE_GRAPHICS) || defined(INTERACTIVE_GRAPHICS_ASCII)
	// random selection if not exporting mode
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(0, materials.size() - 1);
	const int random_num = dist(gen);
	Material selected = materials[random_num];
#endif

#if defined(GRAPHICS) && !defined(INTERACTIVE_GRAPHICS)
	// select in visc order
	int material_idx = repeats / NUM_LOOPS;
	Material selected = materials[int(material_idx)];
#endif

	std::uniform_real_distribution<float> dist2(RPM_RANGE);

	float rpm = 10;
	// print result

	selected.density = 1000;
	selected.surface_tension = 0.042;
	selected.dynamic_viscosity *= MAGNIFY;
	selected.kinematic_viscosity *= MAGNIFY;

	std::cout << "\nSelected Material Properties:\n";
	std::cout << "Name: " << selected.name << "\n";
	std::cout << "Density: " << selected.density << " kg/m^3\n";
	std::cout << "Surface Tension: " << selected.surface_tension << " N/m\n";
	std::cout << "Dynamic Viscosity: " << selected.dynamic_viscosity << " Pa*s\n";
	std::cout << "Kinematic Viscosity: " << selected.kinematic_viscosity << " m^2/s\n";
	std::cout << "RPM: " << rpm << " rad/s\n";

	// si values
	const float si_box = 1.0f;
	const float fan_ratio = 0.4f;
	const float si_omega = rpm * 2.0f * 3.14f;
	const float si_radius = si_box * fan_ratio / 2.0f; // rotor size
	const float si_u = si_radius * si_omega;
	const float si_rho = selected.density;
	const float si_nu = selected.kinematic_viscosity; // water: 0.000001
	const float si_sigma = selected.surface_tension;
	const float si_g = 9.8f;
	const bool enable_cylinder = true;
	

	// lbm reference values
	const uint fps = FPS;
	const uint N = GRID;
	const uint3 lbm_grid = uint3(N, N, N); // Simulation spatial resolution
	const ulong lbm_dt = 1ull; // Simulation time resolution
	const ulong lbm_stop = STOP_CAPTURE;
	const ulong lbm_init = START_CAPTURE;
	const ulong lbm_decay = STOP_ROTOR;
	const float lbm_radius = (float)N * fan_ratio / 2.0f;
	const float lbm_u = lbm_radius * si_omega / fps; // lbm_u = (displacement in grids) / (time variance in # of dt steps)
	const float lbm_rho = 1.0f;	// should be set to 1.0 according to developers

	units.set_m_kg_s(lbm_radius, lbm_u, lbm_rho, si_radius, si_u, si_rho);

	// lbm world parameters (lbm-unit)
	const float lbm_nu = units.nu(si_nu);
	const float lbm_sigma = units.sigma(si_sigma);
	const float lbm_f = units.f(si_rho, si_g);
	const float lbm_omega = units.omega(si_omega), lbm_domega = lbm_omega * lbm_dt;
	LBM lbm(lbm_grid, lbm_nu, 0.0f, 0.0f, -lbm_f, lbm_sigma);
	const float3 center = float3(lbm.center().x, lbm.center().y, 1.0f * lbm_radius);

	// ###################################################################################### define geometry ######################################################################################
	Mesh* mesh = read_stl(get_exe_path() + "../stl/fan.stl", lbm.size(), center, 2.0f * lbm_radius);
	const uint Nx = lbm.get_Nx(), Ny = lbm.get_Ny(), Nz = lbm.get_Nz();

	parallel_for(lbm.get_N(), [&](ulong n) {
		uint x = 0u, y = 0u, z = 0u;
		lbm.coordinates(n, x, y, z);
		int h = Nz / 3u;

		if (z < h) {
			lbm.flags[n] = TYPE_F;
			lbm.rho[n] = units.rho_hydrostatic(lbm_f, (float)z, h);
		}

		if (enable_cylinder == true) {
			if (cylinder(x, y, z, center, float3(0.0f, 0.0f, 300.0f), lbm.get_Nx() / 2u) == 0 && z < Nz - 1u) {
				lbm.flags[n] = TYPE_S;
			}
		}

		if (x == 0u || x == Nx - 1u || y == 0u || y == Ny - 1u || z == 0u)lbm.flags[n] = TYPE_S;
		});

	// ####################################################################### run simulation, export images and data ##########################################################################
#if defined(GRAPHICS) && !defined(INTERACTIVE_GRAPHICS)
	// export settings
	int folder_num = repeats + NAMING_START;
	#ifdef DECAY_MODE
	std::string folder_str = exportPath(get_exe_path(), true, folder_num);
	#endif
	#ifdef STIR_MODE
	std::string folder_str = exportPath(get_exe_path(), false, folder_num);
	#endif

	const string image_path = get_exe_path() + "../export/" + folder_str + "/";
	std::filesystem::create_directories(image_path);
	const string config_path = get_exe_path() + "../export/parameters/";
	const string video_path = get_exe_path() + "../export/videos/";
#endif

	// Initialize simulation
	lbm.graphics.visualization_modes = VIS_PHI_RAYTRACE;
	lbm.run(0u, lbm_stop);

	// simulation loop

	while (lbm.get_t() < lbm_stop) {
#ifdef STIR_MODE
		lbm.voxelize_mesh_on_device(mesh, TYPE_S | TYPE_X, center, float3(0.0f), float3(0.0f, 0.0f, lbm_omega));
		mesh->rotate(float3x3(float3(0.0f, 0.0f, 1.0f), lbm_domega));
#endif
#ifdef DECAY_MODE
		if (lbm.get_t() < lbm_decay) {
			lbm.voxelize_mesh_on_device(mesh, TYPE_S | TYPE_X, center, float3(0.0f), float3(0.0f, 0.0f, lbm_omega));
			mesh->rotate(float3x3(float3(0.0f, 0.0f, 1.0f), lbm_domega));
		}
#endif
		lbm.run(lbm_dt, lbm_stop);

#if defined(GRAPHICS) && !defined(INTERACTIVE_GRAPHICS)
		if (!std::filesystem::exists(config_path)) std::filesystem::create_directories(config_path);
		if (!std::filesystem::exists(video_path)) std::filesystem::create_directories(video_path);
		exportConfig(selected, rpm, CONFIG_OPTION, config_path, folder_str);

		if (lbm.graphics.next_frame(lbm_stop-lbm_init, OUTPUT_TIME, OUTPUT_FPS) && lbm_init < lbm.get_t() && lbm.get_t() < lbm_stop) {
			lbm.graphics.set_camera_free(float3(0.0f * (float)Nx, 0.0f * (float)Ny, 0.4f * (float)Nz), 0.0f, 90.0f, 80.0f);
			lbm.graphics.write_frame(image_path);
		}
#endif // GRAPHICS && !INTERACTIVE_GRAPHICS
	}
	// exportVideo(export_path)  //this function is still on develpment
}