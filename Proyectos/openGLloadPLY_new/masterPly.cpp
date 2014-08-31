#include "masterPly.h"
#include <time.h>

// mesh definition
#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/point_sampling.h>
#include <vcg/complex/algorithms/create/ball_pivoting.h>
#include "plylib.h"

#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/refine.h>

// input output
/**/
#include <wrap/io_trimesh/import_ply.h>
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/export_stl.h>

// std
#include <vector>

using namespace vcg;
using namespace std;
using namespace tri;

class MyVertex; class MyEdge; class MyFace;
struct MyUsedTypes : public vcg::UsedTypes<vcg::Use<MyVertex>   ::AsVertexType,
                                           vcg::Use<MyEdge>     ::AsEdgeType,
                                           vcg::Use<MyFace>     ::AsFaceType>{};
class MyVertex  : public vcg::Vertex< MyUsedTypes, vcg::vertex::Coord3f, vcg::vertex::Normal3f, vcg::vertex::BitFlags  >{};
class MyFace    : public vcg::Face<   MyUsedTypes, vcg::face::FFAdj,  vcg::face::VertexRef, vcg::face::BitFlags > {};
class MyEdge    : public vcg::Edge<   MyUsedTypes> {};
class MyMesh    : public vcg::tri::TriMesh< std::vector<MyVertex>, std::vector<MyFace> , std::vector<MyEdge>  > {};

/*void CSurface(const char input, const char output)
{
	thread t(Sampling,input,output);
	t.join();
}*/

void MasterPly::Sampling (char* input, char* output) {
	MyMesh m;
	MyMesh subM;
	float rad = 0.f;
	int vSize = 0;
	if(tri::io::ImporterPLY<MyMesh>::Open(m,input)!=0)
	{
		cout << "Error reading file  %s\n" << endl;
		return;
	}

	// calculate radius
	rad = tri::SurfaceSampling<MyMesh,tri::TrivialSampler<MyMesh> >::ComputePoissonDiskRadius(m, 10000);
	tri::SurfaceSampling<MyMesh,tri::TrivialSampler<MyMesh> >::SamplingRandomGenerator().initialize((unsigned int)time(0));

	//sample point cloud
	std::vector<Point3f> sampleVec;
	tri::TrivialSampler<MyMesh> mps(sampleVec);

	// sampling
	cout << "Subsampling a PointCloud" << endl;
	tri::SurfaceSampling<MyMesh,tri::TrivialSampler<MyMesh> >::PoissonDiskParam pp;
	tri::SurfaceSampling<MyMesh,tri::TrivialSampler<MyMesh> >::PoissonDiskParam::Stat pds;
	pp.preGenMesh = &subM;
	//pp.pds=&pds;
	pp.bestSampleChoiceFlag=false;

	// start poisson disk prunning
	sampleVec.clear();
	tri::SurfaceSampling<MyMesh,tri::TrivialSampler<MyMesh> >::PoissonDiskPruning(mps, m, rad, pp);
	tri::Build(subM,sampleVec);

	// sample
	std::stringstream sample;
	sample << "sampled_" << output;

	//Build surface
	vcg::tri::UpdateBounding<MyMesh>::Box(subM);
	vcg::tri::UpdateNormal<MyMesh>::PerFace(subM);

	//Initialization
	tri::BallPivoting<MyMesh> pivot(subM);

	//the main processing
	pivot.BuildMesh();

	//output the result
	tri::io::ExporterSTL<MyMesh>::Save(subM,output);
}


MasterPly::MasterPly()
{
    //ctor
}
