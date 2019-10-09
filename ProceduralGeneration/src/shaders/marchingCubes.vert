#version 450 core
layout (location = 0) in vec3 aCellPos;

uniform sampler3D noise;
uniform float cellLength;
uniform float cellsPerAxis;

out vec3 vertexPositions[8];
out float noiseValues[8];

void main() {
    gl_Position = vec4(aCellPos, 1.0);
	
	vertexPositions[0] = vec3(0.0, 0.0, 0.0);
	vertexPositions[1] = vec3(0.0, 0.0, cellLength);
	vertexPositions[2] = vec3(cellLength, 0.0, cellLength);
	vertexPositions[3] = vec3(cellLength, 0.0, 0.0);
	vertexPositions[4] = vec3(0.0, cellLength, 0.0);
	vertexPositions[5] = vec3(0.0, cellLength, cellLength);
	vertexPositions[6] = vec3(cellLength, cellLength, cellLength);
	vertexPositions[7] = vec3(cellLength, cellLength, 0.0);

	for (int i = 0; i < 8; i++) {
		noiseValues[i] = texelFetch(noise,
								    ivec3(round((aCellPos.x + vertexPositions[i].x) / cellLength + cellsPerAxis / 2.0),
										  round((aCellPos.y + vertexPositions[i].y) / cellLength + cellsPerAxis / 2.0),
										  round((aCellPos.z + vertexPositions[i].z) / cellLength + cellsPerAxis / 2.0)), 
								    0).x;
	}
}
