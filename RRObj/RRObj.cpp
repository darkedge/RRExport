// RRObj.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;
using namespace glm;
using namespace Assimp;

int _tmain( int argc, char *argv[] ) {
	Exporter exporter;

	if ( argc != 2 ) {
		cerr << "Invalid number of arguments." << endl;
		return 1;
	}
	char buf[256];
	char bmsh[4];
	char numChars;
	unsigned short numMaterials;

	// C interface for reading
	FILE *inFile = fopen( argv[1] , "rb" );

	string str(argv[1]);
	size_t t = str.find_last_of('.');
	// C++ interface for writing
	ofstream outFile( string(str.substr(0, t)).append(".obj") );

	fread( bmsh, 1, 4, inFile );
	fread( &numMaterials, 2, 1, inFile );
	cout << numMaterials << " materials:" << endl;
	for ( unsigned int i = 0; i < numMaterials; i++ ) {
		fread( &numChars, 1, 1, inFile );
		fread( buf, 1, numChars, inFile );
		printf( "%.*s - ", numChars, buf ); // Material name
		fread( &numChars, 1, 1, inFile );
		fread( buf, 1, numChars, inFile );
		printf( "%.*s.3df\n", numChars, buf ); // Texture name
	}

	cout << endl;
	
	unsigned int index = 1;
	while ( !feof( inFile ) ) {

		/************************************************************************/
		/* READ                                                                 */
		/************************************************************************/

		// Skip 8 bytes
		fread( buf, 1, 8, inFile );
		fread( &numChars, 1, 1, inFile );
		fread( buf, 1, numChars, inFile );
		if ( feof( inFile ) ) break;
		printf( "Object: %.*s\n", numChars, buf ); // Object name

		outFile << "g " /*<< string( buf, numChars ).c_str()*/ << endl;

		// Read vertex data
		unsigned int numVertices;
		fread( &numVertices, 4, 1, inFile );
		struct VertexInfo {
			float x, y, z, u, v;
		};
		VertexInfo *vertexData = new VertexInfo[numVertices];
		fread( vertexData, sizeof( VertexInfo ), numVertices, inFile );

		// Read face data
		unsigned int numFaces;
		fread( &numFaces, 4, 1, inFile );
		struct FaceInfo {
			unsigned short A, B, C, X;
			unsigned int smoothing, material;
		};
		FaceInfo *faceData = new FaceInfo[numFaces];
		fread( faceData, sizeof( FaceInfo ), numFaces, inFile );

		// Skip 9 bytes
		fread( buf, 1, 9, inFile );

		// Print matrix
		mat3 matrix;
		fread( value_ptr( matrix ), sizeof( mat3 ), 1, inFile );

		cout << "Matrix:" << endl;
		for ( unsigned int y = 0; y < 3; y++ ) {
			for ( unsigned int x = 0; x < 3; x++ ) {
				cout << matrix[y][x] << " ";
			}
			cout << endl;
		}

		// Skip 9 bytes
		fread( buf, 1, 9, inFile );

		// Print translation
		vec3 translation;
		//float translation[3];
		fread( value_ptr( translation ), sizeof( vec3 ), 1, inFile );

		cout << "Translation:" << endl;
		for ( unsigned int i = 0; i < 3; i++ ) {
			cout << translation[i] << " ";
		}
		cout << endl;

		/************************************************************************/
		/* CALCULATE                                                            */
		/************************************************************************/
		
#if 1
		for ( unsigned int i = 0; i < numVertices; i++ ) {
			vec3 a(vertexData[i].x, vertexData[i].y, vertexData[i].z);
			a = matrix * a;
			a += translation;
			vertexData[i].x = a.x;
			vertexData[i].y = a.y;
			vertexData[i].z = a.z;
		}
#endif

		/************************************************************************/
		/* WRITE                                                                */
		/************************************************************************/

		// Write vertices
		for ( unsigned int i = 0; i < numVertices; i++ ) {
			outFile << "v " <<
					vertexData[i].x << " " <<
					vertexData[i].y << " " <<
					vertexData[i].z << endl;
		}

		outFile << endl;

		// Write UV's
		for ( unsigned int i = 0; i < numVertices; i++ ) {
			outFile << "vt " <<
					vertexData[i].u << " " <<
					vertexData[i].v << endl;
		}

		outFile << endl;

		// Write faces
		for ( unsigned int i = 0; i < numFaces; i++ ) {
			outFile << "f " <<
					faceData[i].A + index << "/" << faceData[i].A + index << " " <<
					faceData[i].B + index << "/" << faceData[i].B + index << " " <<
					faceData[i].C + index << "/" << faceData[i].C + index << endl;
		}

		outFile << endl;

		delete []faceData;
		delete []vertexData;

		index += numVertices;
	}

	return 0;
}

