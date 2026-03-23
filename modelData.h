#ifndef MODELDATA_H
#define MODELDATA_H


#include <stdint.h>
//#include "collisionUtil.h"
#include <stdbool.h>
#include "mathUtil.h"
#include <string.h>
#include "arena.h"
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

void* modelArenaBackingBuffer;
Arena modelArena;

//https://github.com/KhronosGroup/glTF-Tutorials/blob/main/gltfTutorial/gltfTutorial_020_Skins.md
#ifndef AABB_DEFINED 
// Check if AABB_DEFINED is not defined
#define AABB_DEFINED
typedef struct AABB{
    float xmin;
    float xmax;
    float ymin;
    float ymax;
    float zmin;
    float zmax;
}AABB;

#endif// for checking if this struct has been defined elsewhere 

#ifndef VERTEX_T_DEFINED

#define VERTEX_T_DEFINED
typedef struct Vertex_t{
    Vector3 position;
    Vector3 normal;
    Vector2 uv;
    Vector4 joint_indices;
    Vector4 joint_weights;
}Vertex_t;
#endif

enum modelType{
    CUBE
};
enum interpolationType{
    LINEAR,
    CUBICSPLINE,
    STEP
};
typedef struct Extents Extents;
struct Extents{
    float xMin;
    float xMax;
    float yMin;
    float yMax;
    float zMin;
    float zMax;
};
typedef struct MeshData MeshData;
struct MeshData{
    const float *vertices;
    int numVertices;
    const uint16_t *indices;
    int numIndices;
};

typedef struct{
    float vertices[216];
    uint16_t indices[36];
}Cube;


char *gltfFiles[] = {"Cube","Fox"};
typedef struct skinMatrix_t{
    Mat4 skinMatrix;
}skinMatrix_t;

typedef struct Primitive_t{
    Vertex_t *struct_vertex_buffer;
    size_t buffer_size;
    uint16_t *index_buffer;
    size_t index_buffer_size;
    Vector4 *weights;
    size_t weight_buffer_size;
    Vector4 joints;
    size_t *joint_buffer_size;
    //size_t joint_buffer_size;
}Primitive_t;


typedef struct Mesh_t{
    Primitive_t *primitives;
    int numberOfPrimitives;
}Mesh_t;

typedef struct Node_t{
    int index;
    int parentIndex;
    bool hasParent;
    int *childIndices;
    int numChildren;
    float determinant;
    Mesh_t *meshes;
    int numberOfMeshes;
    Vector3 translation;
    Vector4 rotation;
    Vector3 scale;
    Vector3 homeTranslation;
    Vector4 homeRotation;
    Vector3 homeScale;
    Vector3 transitionTranslation;//USED TO HOLD START/END TRANSFORM
    Vector4 transitionRotation;//USED TO HOLD START/END TRANSFORM
    Vector3 transitionScale;//USED TO HOLD START/END TRANSFORM
    Mat4 transformMatrix;
    Mat4 worldTransformMatrix;
    Mat4 inverseJointMatrix;
    char name[100];
}Node_t;

typedef struct Sampler_t{
    float *sampler_time;
    int num_sampler_time;
    Vector4 *rotation;
    Vector3 *translation;
    Vector3 *scale;
    int sampler_transform_count;
    int interpolation;
}Sampler_t;

typedef struct Channel_t{
    Sampler_t sampler;
    int nodeIndex;
    int transform_type;
}Channel_t;
typedef struct Animation_t{
    char name[100];
    Channel_t *channels;
    int numChannels;
    float animationDuration;
}Animation_t;
typedef struct Material_t{
    bool hasBaseColor;
    bool hasSpecular;
    bool hasMetallic;
    bool hasEmissive;
    bool hasNormal;
    bool hasAmbientOcclusion;

    char baseColorUri[100];
    char specUri[100];
    char metallicUri[100];
    char emmissiveUri[100];
    char normalUri[100];
    char ambientOcclusionUri[100]; 
    
    
}Material_t;
typedef struct ModelBuffers_t{
    //slg_buffer vbuffer;
    int vbuffer_size;
    int *vertexOffsets;
    //slg_buffer ibuffer;
    int ibuffer_size;
    int *indexOffsets;
    int numOffsets;
    Vertex_t *combinedVertBuffer;
    uint16_t *combinedIndexBuffer;
}ModelBuffers_t;

typedef struct Model_t{
    Node_t *nodes;
    int numberOfNodes;
    Animation_t *animations;
    int numberOfAnimations;
    Material_t *materials;
    int numberOfMaterials;
    
}Model_t;

typedef struct Model_List_t
{
    Model_t *models;
    int numberOfModels;
    ModelBuffers_t *modelbuffers;
}Model_List_t;

Model_List_t model_list;

typedef struct{
    Model_t model;
    ModelBuffers_t model_buffers;
}GLTF_Data;

GLTF_Data gltf_data;

Arena modelArena;
void fillModelBuffers(Model_t *model,ModelBuffers_t *modelBuffers);
void nodesDeepCopy(Node_t *src,int srcCount,Node_t *dst,Arena *arena){
    for(int i = 0;i<srcCount;i++){
        dst[i].numChildren = src[i].numChildren;
        dst[i].childIndices = arena_alloc(arena,src[i].numChildren*sizeof(int));
        for(int c = 0;c<dst[i].numChildren;c++){
            dst[i].childIndices[c] = src[i].childIndices[c];
        }
        dst[i].hasParent = src[i].hasParent;
        dst[i].index = src[i].index;
        dst[i].determinant = src[i].determinant;
        dst[i].inverseJointMatrix = src[i].inverseJointMatrix;
        dst[i].parentIndex = src[i].parentIndex;
        dst[i].rotation = src[i].rotation;
        dst[i].scale = src[i].scale;
        dst[i].translation = src[i].translation;
        dst[i].homeRotation = src[i].homeRotation;
        dst[i].homeScale = src[i].homeScale;
        dst[i].homeTranslation = src[i].homeTranslation;
        dst[i].transformMatrix = src[i].transformMatrix;
        dst[i].worldTransformMatrix = src[i].worldTransformMatrix;
        
        strcpy(dst[i].name,src[i].name);
        //memcpy(dst[i].name,src[i].name,100);
        Node_t srcNode = src[i];
        Node_t dstNode = dst[i];
        dst[i].meshes = src[i].meshes;
        dst[i].numberOfMeshes = src[i].numberOfMeshes;
    }
}
void initModelList(){
    int numModels = sizeof(gltfFiles)/sizeof(gltfFiles[0]);
    model_list.models = arena_alloc(&modelArena,numModels*sizeof(Model_t));
    model_list.modelbuffers = arena_alloc(&modelArena,numModels*sizeof(Model_List_t));
    //model_list.models = malloc(numModels * sizeof(Model_t));
    //assert(model_list.models != NULL && "ModelList failed to allocate");
    model_list.numberOfModels = numModels;
}
//Primitive_Data model_data_list = {0};
void fillVertexBuffer(Primitive_t *prim,Vector3 *buffer,int size){
    //prim->vertex_buffer = malloc(size * sizeof(float));
    //assert(prim->vertex_buffer != NULL && "Error allocating vertex buffer");
    //prim->vertex_buffer = arena_alloc(&modelArena,size * sizeof(Vector3));
    //prim->vertex_buffer_size = size;
    if(prim->struct_vertex_buffer == NULL){
        prim->struct_vertex_buffer = arena_alloc(&modelArena,size);
        prim->buffer_size = size;
    }
    //prim->struct_vertex_buffer = arena_alloc(&modelArena,size);
    //prim->buffer_size = size;
    for(int i =0 ;i<size;i++){
        //prim->vertex_buffer[i] = buffer[i];
        prim->struct_vertex_buffer[i].position = buffer[i];

    }
}
void fillNormalBuffer(Primitive_t *prim,Vector3 *buffer, int size){
    //prim->normal_buffer = malloc(size * sizeof(float));
    //prim->normal_buffer = arena_alloc(&modelArena,size * sizeof(float));
    if(prim->struct_vertex_buffer == NULL){
        prim->struct_vertex_buffer = arena_alloc(&modelArena,size);
        prim->buffer_size = size;
    }
    //assert(prim->normal_buffer != NULL && "Error allocating normal buffer");
    //prim->normal_buffer_size=size;
    for(int i =0;i<size;i++){
        prim->struct_vertex_buffer[i].normal = buffer[i];
    }
}
void fillIndexBuffer(Primitive_t *prim,int *buffer,int size){
    //prim->index_buffer = malloc(size*sizeof(uint16_t));
    //assert(prim->index_buffer != NULL && "Error allocating index_buffer");
    prim->index_buffer = arena_alloc(&modelArena,size*sizeof(uint16_t));
    prim->index_buffer_size = size;
    for(int i=0;i<size;i++){
        prim->index_buffer[i] = buffer[i];
    }
}
void calculateNormalsFromVertex(Vertex_t *buffer,int numVerts){
    for(int i=0;i<numVerts;i+=3){ //pulling out 3 float per vert for 3 verts
        Vector3 A = buffer[i].position;
        Vector3 B = buffer[i+1].position;
        Vector3 C = buffer[i+2].position;

        //normals for A
        //n := normalize(cross(B-A, C-A))
        Vector3 normA = normalizeVec3(crossVec3(subtractVec3(B,A),subtractVec3(C,A)));
        //Vector3 normB = normalizeVec3(crossVec3(subtractVec3(C,B),subtractVec3(A,B)));
        //Vector3 normC = normalizeVec3(crossVec3(subtractVec3(B,C),subtractVec3(A,C)));
        buffer[i].normal = normA;
        
        buffer[i+1].normal = normA;

        buffer[i+2].normal = normA;
        
        /*
        out_normals[i+3] = normB.x;
        out_normals[i+4] = normB.y;
        out_normals[i+5] = normB.z;

        out_normals[i+6] = normC.x;
        out_normals[i+7] = normC.y;
        out_normals[i+8] = normC.z;
        */
        
    }
}
/*void createCombinedBuffer(Primitive_t *prim){
    int bufferSize = prim->vertex_buffer_size + prim->normal_buffer_size;

    //prim->combined_vert_buffer = malloc(sizeof(float) * bufferSize);
    //assert(prim->combined_vert_buffer != NULL && "Problems allocating the combined buffer");

    prim->combined_vert_buffer = arena_alloc(&modelArena,sizeof(float) * bufferSize);
    prim->combined_vert_buffer_size = bufferSize;
    int t = 0;
    for(int i = 0;i<bufferSize;i+=6){
        prim->combined_vert_buffer[i] = prim->vertex_buffer[t+2];
        prim->combined_vert_buffer[i+1] = prim->vertex_buffer[t+1];
        prim->combined_vert_buffer[i+2] = prim->vertex_buffer[t];
        prim->combined_vert_buffer[i+3] = prim->normal_buffer[t];
        prim->combined_vert_buffer[i+4] = prim->normal_buffer[t+1];
        prim->combined_vert_buffer[i+5] = prim->normal_buffer[t+2];
        t+=3;
    }
    printf("debug pause");
}*/
void parsePrimitives(cgltf_primitive *primitives,int numPrimitives,Mesh_t *mesh,cgltf_skin *skin,cgltf_data *data){
    for(int i =0;i<numPrimitives;i++){
        cgltf_primitive primitive = primitives[i];
        cgltf_attribute *attributes = primitive.attributes;
        //fillIndexBuffer(&cube_data.primitive_data[i],p,);
        bool hasNormals = false;
        for(int t=0;t<primitive.attributes_count;t++){
            
            cgltf_attribute attribute = attributes[t];
            if(attributes[t].type == cgltf_attribute_type_position){
                cgltf_accessor *attributeAccessor = attribute.data;
                int accessorSize = attributeAccessor->count;
                //MULT ACCESSOR SIZE BY 3 BECAUSE VEC3
                //cgltf_float *positionFloat = malloc((accessorSize*3)*sizeof(float));
                //assert(positionFloat!=NULL && "error malloc position float");
                Vector3 *positionVec = arena_alloc(&modelArena,(accessorSize)*sizeof(Vector3));
                cgltf_float *positionFloat = arena_alloc(&modelArena,(accessorSize*3)*sizeof(float));
                
                cgltf_size size = cgltf_accessor_unpack_floats(attributeAccessor,positionFloat,accessorSize*3);
                memcpy(positionVec,positionFloat,size*sizeof(float));

                //fillVertexBuffer(&mesh->primitives[i],positionVec,accessorSize);
                if(mesh->primitives[i].struct_vertex_buffer == NULL){
                    mesh->primitives[i].struct_vertex_buffer = arena_alloc(&modelArena,accessorSize*sizeof(Vertex_t));
                    mesh->primitives[i].buffer_size = accessorSize;
                }
                //prim->struct_vertex_buffer = arena_alloc(&modelArena,size);
                //prim->buffer_size = size;
                for(int p =0 ;p<accessorSize;p+=3){
    
                    mesh->primitives[i].struct_vertex_buffer[p].position = positionVec[p];
                    mesh->primitives[i].struct_vertex_buffer[p+1].position = positionVec[p+1];
                    mesh->primitives[i].struct_vertex_buffer[p+2].position = positionVec[p+2];

                }
                //free(positionFloat);
            }
            else if(attributes[t].type == cgltf_attribute_type_normal){
                hasNormals = true;
                cgltf_accessor *attributeAccessor = attribute.data;
                int accessorSize = attributeAccessor->count;
                //MULT ACCESSOR SIZE BY 3 BECAUSE VEC3
                //cgltf_float *normalFloat = malloc((accessorSize*3)*sizeof(float));
                //assert(normalFloat!=NULL && "error malloc position float");

                cgltf_float *normalFloat = arena_alloc(&modelArena,(accessorSize*3)*sizeof(float));
                Vector3 *normalVec = arena_alloc(&modelArena,(accessorSize)*sizeof(Vector3));
                cgltf_size size = cgltf_accessor_unpack_floats(attributeAccessor,normalFloat,(accessorSize*3));
                memcpy(normalVec,normalFloat,size*sizeof(float));
                if(mesh->primitives[i].struct_vertex_buffer == NULL){
                    mesh->primitives[i].struct_vertex_buffer = arena_alloc(&modelArena,accessorSize*sizeof(Vertex_t));
                    mesh->primitives[i].buffer_size = accessorSize;
                }
                //prim->struct_vertex_buffer = arena_alloc(&modelArena,size);
                //prim->buffer_size = size;
                for(int p =0 ;p<accessorSize;p+=3){
    
                    mesh->primitives[i].struct_vertex_buffer[p].normal= normalVec[p];
                    mesh->primitives[i].struct_vertex_buffer[p+1].normal = normalVec[p+1];
                    mesh->primitives[i].struct_vertex_buffer[p+2].normal = normalVec[p+2];

                }
                //fillNormalBuffer(&mesh->primitives[i],normalVec,size);
                //free(normalFloat);
    
            }
            else if(attributes[t].type == cgltf_attribute_type_texcoord){
                cgltf_accessor *attributeAccessor = attribute.data;
                int accessorSize = attributeAccessor->count;
                cgltf_float *uvFloat = arena_alloc(&modelArena,(accessorSize*2)*sizeof(float));
                Vector2 *uvVec = arena_alloc(&modelArena,(accessorSize)*sizeof(Vector2));
                cgltf_size size = cgltf_accessor_unpack_floats(attributeAccessor,uvFloat,(accessorSize*2));
                memcpy(uvVec,uvFloat,size*sizeof(float));
                
                if(mesh->primitives->struct_vertex_buffer == NULL){
                    mesh->primitives[i].struct_vertex_buffer = arena_alloc(&modelArena,accessorSize*sizeof(Vertex_t));
                    mesh->primitives[i].buffer_size = accessorSize;
                }
                //prim->struct_vertex_buffer = arena_alloc(&modelArena,size);
                //prim->buffer_size = size;
                for(int p =0 ;p<accessorSize;p+=3){
                    //Vector2 test = uvVec[p];
                    mesh->primitives[i].struct_vertex_buffer[p].uv = uvVec[p];
                    mesh->primitives[i].struct_vertex_buffer[p+1].uv = uvVec[p+1];
                    mesh->primitives[i].struct_vertex_buffer[p+2].uv = uvVec[p+2];

                   // mesh->primitives->struct_vertex_buffer[i].uv = uvVec[i];
                }
            }
            else if(attributes[t].type == cgltf_attribute_type_weights){
                cgltf_accessor *attributeAccessor = attribute.data;
                int accessorSize = attributeAccessor->count;
                cgltf_float *weightFloat = arena_alloc(&modelArena,(accessorSize*4)*sizeof(float));
                Vector4 *weightVectors = arena_alloc(&modelArena,accessorSize*sizeof(Vector4));
                cgltf_size size = cgltf_accessor_unpack_floats(attributeAccessor,weightFloat,accessorSize*4);
                memcpy(weightVectors,weightFloat,size*sizeof(float));

                if(mesh->primitives[i].struct_vertex_buffer == NULL){
                    mesh->primitives[i].struct_vertex_buffer = arena_alloc(&modelArena,accessorSize*sizeof(Vertex_t));
                    mesh->primitives[i].buffer_size = accessorSize;
                }

                for(int p=0;p<accessorSize;p+=3){
         
                    mesh->primitives[i].struct_vertex_buffer[p].joint_weights = weightVectors[p];
                    mesh->primitives[i].struct_vertex_buffer[p+1].joint_weights = weightVectors[p+1];
                    mesh->primitives[i].struct_vertex_buffer[p+2].joint_weights = weightVectors[p+2];
                }
                
                //mesh->primitives[i].weights = arena_alloc(&modelArena,accessorSize*sizeof(Vector4));
                //mesh->primitives[i].weight_buffer_size = accessorSize;
                //for(int p=0;p<accessorSize;p++){
                //    Vector4 test = weightVectors[p];
                //    printf("debugging");
                //}
            }
            else if(attributes[t].type == cgltf_attribute_type_joints){
                cgltf_accessor *attributeAccessor = attribute.data;
                int accessorSize = attributeAccessor->count;
        
                //cgltf_size required_uints = cgltf_accessor_read_uint(attributeAccessor, 0, NULL, 0);
                //cgltf_size required_uints = cgltf_accessor_read_uint(attributeAccessor, 0, NULL, sizeof(cgltf_uint));
                cgltf_float *jointIndex = arena_alloc(&modelArena,(accessorSize*4)*sizeof(cgltf_float));
                //cgltf_uint *jointIndex_uint = arena_alloc(&modelArena,(accessorSize*4)*sizeof(cgltf_uint));
                Vector4 *jointVectors = arena_alloc(&modelArena,accessorSize*sizeof(Vector4));
                //for(int v=0;v<accessorSize;v++){
                //    cgltf_accessor_read_uint(attributeAccessor,0,jointIndex_uint,accessorSize);
                //}
                //cgltf_accessor_read_uint(attributeAccessor,0,jointIndex_uint,accessorSize);
                
                
                cgltf_size size = cgltf_accessor_unpack_floats(attributeAccessor,jointIndex,accessorSize*4);
                //for(int ui=0;ui<accessorSize*4;ui++){
                //    cgltf_uint test = jointIndex[ui];
                //    printf("debug");
                //}
                //cgltf_float test= jointIndex[4500];
                //test = jointIndex[4501];
                //test = jointIndex[4502];
                //test = jointIndex[4503];

                memcpy(jointVectors,jointIndex,size*sizeof(float));
                for(int z = 0;z<accessorSize;z++){
                    Vector4 testVector = jointVectors[z];
                    if(testVector.x == 0){
                        printf("debug");
                    }
                }
                
                if(mesh->primitives[i].struct_vertex_buffer == NULL){
                    mesh->primitives[i].struct_vertex_buffer = arena_alloc(&modelArena,accessorSize*sizeof(Vertex_t));
                    mesh->primitives[i].buffer_size = accessorSize;
                }

                /*for(int p=0;p<accessorSize;p+=3){
                    
                    mesh->primitives[i].struct_vertex_buffer[p].joint_indices = jointVectors[p];
                    mesh->primitives[i].struct_vertex_buffer[p+1].joint_indices = jointVectors[p+1];
                    mesh->primitives[i].struct_vertex_buffer[p+2].joint_indices = jointVectors[p+2];
                }*/
                for(int p=0;p<accessorSize;p++){
                    Vector4 jointFinalVec;
                    jointFinalVec = jointVectors[p];
                    if(jointVectors[p].x!=0){
                        cgltf_node *tempNode = skin->joints[(int)jointVectors[p].x];
                        int nodeIndex = cgltf_node_index(data,tempNode);
                        jointFinalVec.x = nodeIndex;
                    }
                    if(jointVectors[p].y!=0){
                        cgltf_node *tempNode = skin->joints[(int)jointVectors[p].y];
                        int nodeIndex = cgltf_node_index(data,tempNode);
                        jointFinalVec.y = nodeIndex;
                    }
                    if(jointVectors[p].z!=0){
                        cgltf_node *tempNode = skin->joints[(int)jointVectors[p].z];
                        int nodeIndex = cgltf_node_index(data,tempNode);
                        jointFinalVec.z = nodeIndex;
                    }
                    if(jointVectors[p].w!=0){
                        cgltf_node *tempNode = skin->joints[(int)jointVectors[p].w];
                        int nodeIndex = cgltf_node_index(data,tempNode);
                        jointFinalVec.w = nodeIndex;
                    }
                    //in nodeIndex = skin->joints[joint]
    
                    mesh->primitives[i].struct_vertex_buffer[p].joint_indices = jointFinalVec;
                }
            }

        }
        if(primitive.indices!=NULL){
            //cgltf_uint *indexBuffer = malloc(primitive.indices->count*sizeof(cgltf_uint));
            //assert(indexBuffer != NULL && "malloc failed");
            cgltf_uint *indexBuffer = arena_alloc(&modelArena,primitive.indices->count*sizeof(cgltf_uint));

            int numIndices = cgltf_accessor_unpack_indices(primitive.indices,indexBuffer,sizeof(cgltf_uint),primitive.indices->count);
            fillIndexBuffer(&mesh->primitives[i],indexBuffer,numIndices);
            //free(indexBuffer);
        }
        else{
            int numIndices = mesh->primitives[i].buffer_size;
            //cgltf_uint *indexBuffer = malloc(numIndices*sizeof(cgltf_uint));
            //assert(indexBuffer != NULL && "malloc failed");
            cgltf_uint *indexBuffer = arena_alloc(&modelArena,numIndices*sizeof(cgltf_uint));
            for(int i =0;i<numIndices;i++){
                indexBuffer[i]=i;
            }
            fillIndexBuffer(&mesh->primitives[i],indexBuffer,numIndices);
            //free(indexBuffer);
        }
        if(hasNormals == false){
            //int size = mesh->primitives[i].vertex_buffer_size;
            int size = mesh->primitives[i].buffer_size;
            //mesh->primitives[i].normal_buffer = malloc(size*sizeof(float));
            if(mesh->primitives[i].struct_vertex_buffer == NULL){
                mesh->primitives[i].struct_vertex_buffer = arena_alloc(&modelArena,size*sizeof(Vertex_t));
            }
            
            calculateNormalsFromVertex(mesh->primitives[i].struct_vertex_buffer,size);
            printf("test");
            //mesh->primitives->normal_buffer_size = size;
        }
        //createCombinedBuffer(&mesh->primitives[i]);
    }
}
void parseMesh(cgltf_mesh *c_mesh,Node_t *node,cgltf_skin *skin,cgltf_data *data){
    node->numberOfMeshes = 1;
    node->meshes = arena_alloc(&modelArena,sizeof(Mesh_t));
    //node->meshes = malloc(1*sizeof(Mesh_t));
    //assert(node->meshes != NULL && "issue mallocing");
    int numberOfPrimitives = c_mesh->primitives_count;
    
    cgltf_primitive *primitives = c_mesh->primitives;
    
    for(int i =0;i<node->numberOfMeshes;i++){
        node->meshes[i].numberOfPrimitives = numberOfPrimitives;
        //node->meshes[i].primitives = malloc(numberOfPrimitives * sizeof(Primitive_t));
        //assert(node->meshes[i].primitives != NULL && "mesh.primitives allocation failed");
        node->meshes[i].primitives = arena_alloc(&modelArena,numberOfPrimitives * sizeof(Primitive_t));
        parsePrimitives(primitives,numberOfPrimitives,&node->meshes[i],skin,data);
    }
}
void parseNodes(cgltf_node *nodes,cgltf_data *data,Model_t *model){
    int numberOfNodes = data->nodes_count;
    //allocate the space for the nodes inside of the model
    int nodeIndex=0;
    model->numberOfNodes = numberOfNodes;
    model->nodes = arena_alloc(&modelArena,sizeof(Node_t)*numberOfNodes);
    //model->nodes = malloc(numberOfNodes * sizeof(Node_t));
    //assert(model->nodes != NULL && "model nodes failed to allocate");
    for(int i =0;i<numberOfNodes;i++){
        cgltf_node *c_node = &nodes[i];
        strncpy(model->nodes[i].name,c_node->name,100); 
        nodeIndex = (int)(c_node - nodes);
        model->nodes[nodeIndex].index = nodeIndex;
        if(c_node->parent==NULL){
            //Node_t node;
            //model->nodes[nodeIndex].transformMatrix = c_node.matrix;
            cgltf_node_transform_local(c_node,model->nodes[nodeIndex].transformMatrix.Elements_16);
            cgltf_node_transform_world(c_node,model->nodes[nodeIndex].worldTransformMatrix.Elements_16);
            memcpy(model->nodes[nodeIndex].translation.Elements,c_node->translation,3*sizeof(float));
            memcpy(model->nodes[nodeIndex].scale.Elements,c_node->scale,3*sizeof(float));
            memcpy(model->nodes[nodeIndex].rotation.Elements,c_node->rotation,4*sizeof(float));
            
            model->nodes[nodeIndex].homeTranslation =  model->nodes[nodeIndex].translation;
            model->nodes[nodeIndex].homeRotation =  model->nodes[nodeIndex].rotation;
            model->nodes[nodeIndex].homeScale =  model->nodes[nodeIndex].scale;
            model->nodes[nodeIndex].hasParent = false;
            //for(int t =0;t<16;t++){
            //    model->nodes[nodeIndex].transformMatrix.Elements_16[t] = c_node->matrix[t];
            //    model->nodes[nodeIndex].worldTransformMatrix.Elements_16[t] = c_node->matrix[t];
            //}

            if(c_node->mesh!=NULL){
                cgltf_mesh *mesh = c_node->mesh;
                parseMesh(mesh,&model->nodes[nodeIndex],c_node->skin,data);
            }
        }
        else{
            model->nodes[nodeIndex].hasParent = true;
            model->nodes[nodeIndex].parentIndex = (int)(c_node->parent - nodes);
        }
        
        int numberOfChildren = c_node->children_count;
        model->nodes[nodeIndex].numChildren = numberOfChildren;
        if(numberOfChildren>0){
            int parentIndex = nodeIndex;
            //model->nodes[i].children = malloc(numberOfChildren*sizeof(Node_t));
            //assert(model->nodes[i].children != NULL && "problem allocating child indices");
            model->nodes[parentIndex].childIndices = arena_alloc(&modelArena,numberOfChildren*sizeof(int));
            for(int c=0;c<numberOfChildren;c++){
               // model->nodes[i].children[c] = c_node.children[i]; 
                cgltf_node *child_node = c_node->children[c];                
                nodeIndex = (int)(child_node - nodes);
                model->nodes[parentIndex].childIndices[c] = nodeIndex;
                if(child_node->mesh!=NULL){
                    cgltf_mesh *mesh = child_node->mesh;
                    parseMesh(mesh,&model->nodes[nodeIndex],child_node->skin,data);
                }
                cgltf_node_transform_local(child_node,model->nodes[nodeIndex].transformMatrix.Elements_16);
                cgltf_node_transform_world(child_node,model->nodes[nodeIndex].worldTransformMatrix.Elements_16);
                bool hasTranslation = child_node->has_translation;
                memcpy(model->nodes[nodeIndex].translation.Elements,child_node->translation,3*sizeof(float));
                memcpy(model->nodes[nodeIndex].scale.Elements,child_node->scale,3*sizeof(float));
                memcpy(model->nodes[nodeIndex].rotation.Elements,child_node->rotation,4*sizeof(float));
                model->nodes[nodeIndex].homeTranslation =  model->nodes[nodeIndex].translation;
                model->nodes[nodeIndex].homeRotation =  model->nodes[nodeIndex].rotation;
                model->nodes[nodeIndex].homeScale =  model->nodes[nodeIndex].scale;

                
                
                //for(int t =0;t<16;t++){
                //    model->nodes[nodeIndex].transformMatrix.Elements_16[t] = child_node->matrix[t];
                //}
                //Mat4 test = model->nodes[nodeIndex].transformMatrix;
                //Mat4 test_worldTransorm = mulMat4(model->nodes[nodeIndex].transformMatrix,model->nodes[parentIndex].worldTransformMatrix);
                //model->nodes[nodeIndex].worldTransformMatrix = mulMat4(model->nodes[nodeIndex].transformMatrix,model->nodes[parentIndex].worldTransformMatrix);
                //cgltf_node *parent_node = c_node->parent;
                //model->nodes[nodeIndex].worldTransformMatrix = mulMat4(model->nodes[nodeIndex].transformMatrix);

            }
        }
        Node_t testNode = model->nodes[i];
        printf("break");
    }
}
Vector4 interpolateAnim(int interpolateEnum,float currentTime,float previousTime,float nextTime,Vector4 previousTransform,Vector4 nextTransform){
    switch (interpolateEnum){
        case cgltf_interpolation_type_linear:
            break;
        case cgltf_interpolation_type_cubic_spline:
            break;
        default:
            break;
    }
    return (Vector4){0,0,0,0};
}
void parseAnimations(cgltf_animation *animations,int animationCount,cgltf_data *data,Model_t *model){
    model->animations = arena_alloc(&modelArena,animationCount*sizeof(Animation_t));
    
    model->numberOfAnimations = animationCount;
    for(int i = 0;i<animationCount;i++){
        float animationDuration=0;
        float animationMin=0;
        float animationMax = 0;
        //char* test = animations[i].name;
        //model->animations[i].name = animations[i].name;
        strcpy(model->animations[i].name,animations[i].name);
        int numChannels = animations[i].channels_count;
        model->animations[i].channels = arena_alloc(&modelArena,numChannels*sizeof(Channel_t));
        model->animations[i].numChannels = numChannels;
        

        for(int c = 0;c<numChannels;c++){
            cgltf_animation_channel channel = animations[i].channels[c];
            cgltf_node *node = channel.target_node;
            int nodeIndex = cgltf_node_index(data,node);
            model->animations[i].channels[c].nodeIndex = nodeIndex;
            
            //model->animations[i].channels[c].numSamplers
            cgltf_animation_sampler *sampler = channel.sampler;
            
            
            cgltf_accessor *input = sampler->input;
            
            if(c == 0){
                animationMin = *input->min;
                animationMax = *input->max;
            }
            else{
                if(*input->min < animationMin){
                    animationMin = *input->min;
                }
                if(*input->max > animationMax){
                    animationMax = *input->max;
                }
            }
            int numberOfInputs = input->count;
            model->animations[i].channels[c].sampler.sampler_time = arena_alloc(&modelArena,numberOfInputs*sizeof(float));
            model->animations[i].channels[c].sampler.num_sampler_time = numberOfInputs;
            //float *inputFloats = arena_alloc(&modelArena,numberOfInputs*sizeof(float));
            cgltf_accessor_unpack_floats(input,model->animations[i].channels[c].sampler.sampler_time,numberOfInputs);

            cgltf_accessor *output = sampler->output;
            int numberOfOutputs = output->count;
            float *outputFloats;
            int size = 0;
            model->animations[i].channels[c].transform_type = channel.target_path;
            switch(channel.target_path){
                case cgltf_animation_path_type_translation:
                    model->animations[i].channels[c].sampler.translation = arena_alloc(&modelArena,numberOfOutputs*sizeof(Vector3));
                    outputFloats = arena_alloc(&modelArena,numberOfOutputs*3*sizeof(float));
                    size = cgltf_accessor_unpack_floats(output,outputFloats,numberOfOutputs*3);
                    memcpy(model->animations[i].channels[c].sampler.translation,outputFloats,size*sizeof(float));
                    break;
                case cgltf_animation_path_type_scale:
                    model->animations[i].channels[c].sampler.scale = arena_alloc(&modelArena,numberOfOutputs*sizeof(Vector3));
                    outputFloats = arena_alloc(&modelArena,numberOfOutputs*3*sizeof(float));
                    size = cgltf_accessor_unpack_floats(output,outputFloats,numberOfOutputs*3);
                    memcpy(model->animations[i].channels[c].sampler.scale,outputFloats,size*sizeof(float));
                    break;
                case cgltf_animation_path_type_rotation:
                    model->animations[i].channels[c].sampler.rotation = arena_alloc(&modelArena,numberOfOutputs*sizeof(Vector4));
                    outputFloats = arena_alloc(&modelArena,numberOfOutputs*4*sizeof(float));
                    size = cgltf_accessor_unpack_floats(output,outputFloats,numberOfOutputs*4);
                    memcpy(model->animations[i].channels[c].sampler.rotation,outputFloats,size*sizeof(float));
                    break;
                case cgltf_animation_path_type_invalid:
                    assert(false && "Something wrong with path type");
                    break;
                default:
                    break;
            }
            //Vector4 *outputVec = arena_alloc(&modelArena,numberOfOutputs*sizeof(Vector4));
            //model->animations[i].channels[c].sampler.sampler_transform = arena_alloc(&modelArena,numberOfOutputs*sizeof(Vector4));
            //model->animations[i].channels[c].sampler.sampler_transform_count = numberOfOutputs;
            //model->animations[i].channels[c].sampler.interpolation = sampler->interpolation;
            //float *outputFloats = arena_alloc(&modelArena,numberOfOutputs*4*sizeof(float));
            //int size = cgltf_accessor_unpack_floats(output,outputFloats,numberOfOutputs*4);
            //memcpy(model->animations[i].channels[c].sampler.sampler_transform,outputFloats,size*sizeof(float));
            
            
            printf("debug");
            
        }
       
        //int numSamplers = animations[i].samplers_count;
        animationDuration = animationMax-animationMin;
        model->animations[i].animationDuration = animationDuration;
        
        
        
        printf("pausing for debug");
    }
}
void parseSkins(cgltf_skin *skins, int skinCount,Model_t *model){
    for(int i=0;i<skinCount;i++){
        cgltf_accessor *bind_matrix = skins[i].inverse_bind_matrices;
        int accessorSize = bind_matrix->count;
        cgltf_float *bindMatrixFloats = arena_alloc(&modelArena,accessorSize*sizeof(float)*16);
        Mat4 *bindMatrices = arena_alloc(&modelArena,accessorSize*sizeof(Mat4));
        int size = cgltf_accessor_unpack_floats(bind_matrix,bindMatrixFloats,accessorSize*16);
        memcpy(bindMatrices,bindMatrixFloats,size*sizeof(float));

        int jointCount = skins[i].joints_count;
        cgltf_node **joints = skins[i].joints;
        for(int m=0;m<model->numberOfNodes;m++){
           
            model->nodes[m].inverseJointMatrix = identityMat4Const;
            
        }
        for(int j = 0;j<jointCount;j++){
            
            for(int m=0;m<model->numberOfNodes;m++){
                if(strcmp(model->nodes[m].name,joints[j]->name) == 0){
                    model->nodes[m].inverseJointMatrix = bindMatrices[j];
                }
                
            }
        }   
        printf("pause");
        //cgltf_accessor *joint_matrix = skins[i].joints;
        //int accessorSize = joint_matrix->count;
        //cgltf_float *bindMatrixFloats = arena_alloc(&modelArena,accessorSize*sizeof(float));
        
        /*cgltf_float *jointIndex = arena_alloc(&modelArena,(accessorSize*4)*sizeof(cgltf_float));
        Vector4 *jointVectors = arena_alloc(&modelArena,accessorSize*sizeof(Vector4));
        cgltf_accessor_unpack_floats(attributeAccessor,jointIndex,accessorSize*4);
        memcpy(jointVectors,jointIndex,accessorSize*sizeof(float));
        for(int p=0;p<accessorSize;p++){
            Vector4 test = jointVectors[p];
            printf("pause");
        }*/

        
        //for(int p=0;p<accessorSize;p++){
        //    Mat4 matrix = bindMatrices[p];
        //    printf("pause");
        //}
        //printf("pause");
        //skins[i].inverse_bind_matrices;
    }

}
void parseMaterials(cgltf_material *materials, int numMaterials,Model_t *model){
    model->materials = arena_alloc(&modelArena,numMaterials * sizeof(Material_t));
    model->numberOfMaterials = numMaterials;
    for(int i = 0;i<numMaterials;i++){
        cgltf_material material = materials[i];
        
        //material.specular
        if(material.has_pbr_metallic_roughness){
            cgltf_pbr_metallic_roughness pbr = material.pbr_metallic_roughness;
            if(pbr.metallic_roughness_texture.texture != NULL){
                model->materials[i].hasMetallic = true;
                strcpy(model->materials->metallicUri,pbr.base_color_texture.texture->image->uri);
            }
            if(pbr.base_color_texture.texture != NULL){
                model->materials[i].hasBaseColor = true;
                strcpy(model->materials[i].baseColorUri,pbr.base_color_texture.texture->image->uri);
            }
            
            //cgltf_texture_view base_color = pbr.base_color_texture;
            
            //base_color.texture->image->uri;
        }
        if(material.has_specular){
            cgltf_specular spec = material.specular;
            model->materials[i].hasSpecular = true;
            strcpy(model->materials[i].specUri,spec.specular_texture.texture->image->uri);
            //model->materials[i].specUri
        }   
        if(material.emissive_texture.texture != NULL){
            model->materials[i].hasEmissive = true;
            strcpy(model->materials[i].emmissiveUri,material.emissive_texture.texture->image->uri);
        }
        if(material.occlusion_texture.texture!=NULL){
            model->materials[i].hasAmbientOcclusion = true;
            strcpy(model->materials[i].ambientOcclusionUri,material.occlusion_texture.texture->image->uri);
        }
        if(material.normal_texture.texture != NULL){
            model->materials[i].hasNormal = true;
            strcpy(model->materials[i].normalUri,material.normal_texture.texture->image->name);
        }

    }
}
//WE GOT AN ISSUE WITH MEMORY ALLOCATION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// i dont think we have a memory allocation issue anymore but i can investigate if it's still a problem
//instead of passing in an index to the file in an already created list we can just pass in the path because that's how i will be using this now
GLTF_Data getDataFromGltf(char *path, int path_size){

    int arenaSize = 524288;
    void* modelArenaBackingBuffer = malloc(arenaSize);
    
    arena_init(&modelArena,modelArenaBackingBuffer,arenaSize);

    void* buf; /* Pointer to glb or gltf file data */
    size_t size; /* Size of the file data */
    FILE *file = fopen(path,"rb");
    assert(file != NULL && "Error opening gltf file");

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    buf = malloc(size);
    fread(buf, 1, size, file);
    fclose(file);
    cgltf_options options = {0};
    cgltf_data* data = NULL;
    cgltf_result result = cgltf_parse(&options, buf, size, &data);
    
    Model_t model;
    if (result == cgltf_result_success)
    {
        result = cgltf_load_buffers(&options, data, path); // base path
        if (result != cgltf_result_success) {
            printf("Failed to load external buffers\n");
            cgltf_free(data);
            free(buf);
            return (GLTF_Data){0};
        }
        cgltf_node *nodes = data->nodes;
        
        //model.nodes = malloc(data->nodes_count * sizeof(Node_t));
        //assert(model.nodes != NULL && "failure allocating space for nodes");
        //model.numberOfNodes = data->nodes_count;
        parseNodes(nodes,data,&model);
        parseAnimations(data->animations,data->animations_count,data,&model);
        parseSkins(data->skins,data->skins_count,&model);
        parseMaterials(data->materials,data->materials_count,&model);
        for(int i =0;i<data->images_count;i++){
            cgltf_image testImage = data->images[i];
            
            printf("pause");
        }
      
    }
    ModelBuffers_t model_buffers;
    fillModelBuffers(&model,&model_buffers);
    gltf_data.model = model;
    gltf_data.model_buffers = model_buffers;
    cgltf_free(data);
    free(buf);
    return gltf_data;
}
void getNumberOfIndicesInModel(Model_t model,size_t *numIndices){
    *numIndices = 0;
    int numberOfNodes = model.numberOfNodes;
    for(int n = 0; n < numberOfNodes; n++){
        
        int numberOfMeshes = model.nodes[n].numberOfMeshes;
        for(int m = 0;m<numberOfMeshes;m++){
           int numberOfPrimitives =  model.nodes[n].meshes[m].numberOfPrimitives;
           for(int p = 0;p<numberOfPrimitives;p++){
                if(model.nodes[n].meshes[m].primitives[p].index_buffer != NULL){
                    *numIndices += model.nodes[n].meshes[m].primitives[p].index_buffer_size;
                }
                
           }
        }
    } 
}
void fillCombinedIndexBuffer(uint16_t *buff,uint16_t size,Model_t model,int *offsetList){
    
    int offsetIndex=0;
    int indexBufferOffset = 0;
    int numberOfNodes = model.numberOfNodes;
    for(int n = 0; n < numberOfNodes; n++){
        int numberOfMeshes = model.nodes[n].numberOfMeshes;
        for(int m = 0;m<numberOfMeshes;m++){
           int numberOfPrimitives =  model.nodes[n].meshes[m].numberOfPrimitives;
           for(int p = 0;p<numberOfPrimitives;p++){
                if(model.nodes[n].meshes[m].primitives[p].index_buffer != NULL){
                    int smallIndexSize = model.nodes[n].meshes[m].primitives[p].index_buffer_size;
                    for(int i =0;i<smallIndexSize;i++){
                        int test = model.nodes[n].meshes[m].primitives[p].index_buffer[i];
                        printf("debugging");
                    }
                    memcpy((char*)buff + indexBufferOffset,model.nodes[n].meshes[m].primitives[p].index_buffer,smallIndexSize*sizeof(uint16_t));
                    indexBufferOffset += smallIndexSize;
                    offsetList[offsetIndex] = indexBufferOffset;
                    offsetIndex += 1;
                    //model.nodes[n].meshes[m].primitives[p].index_buffer
                }
                
           }
        }
    } 
}
void getNumberOfVertsInModel(Model_t model,size_t *numIndices,int *numOffsets){
    *numOffsets = 0;
    *numIndices = 0;
    int numberOfNodes = model.numberOfNodes;
    for(int n = 0; n < numberOfNodes; n++){
        
        int numberOfMeshes = model.nodes[n].numberOfMeshes;
        for(int m = 0;m<numberOfMeshes;m++){
           int numberOfPrimitives =  model.nodes[n].meshes[m].numberOfPrimitives;
           for(int p = 0;p<numberOfPrimitives;p++){
                if(model.nodes[n].meshes[m].primitives[p].struct_vertex_buffer != NULL){
                    *numIndices += model.nodes[n].meshes[m].primitives[p].buffer_size;
                    *numOffsets += 1;
                }
           }
        }
    } 
}
void fillCombinedVertBuffer(Vertex_t *buff,uint16_t size,Model_t model,int *offsetList){
    int vertBufferOffset = 0;
    int offsetIndex=0;
    int numberOfNodes = model.numberOfNodes;
    for(int n = 0; n < numberOfNodes; n++){
        
        int numberOfMeshes = model.nodes[n].numberOfMeshes;
        for(int m = 0;m<numberOfMeshes;m++){
           int numberOfPrimitives =  model.nodes[n].meshes[m].numberOfPrimitives;
           for(int p = 0;p<numberOfPrimitives;p++){
                if(model.nodes[n].meshes[m].primitives[p].struct_vertex_buffer != NULL){
                    Vertex_t *vert = model.nodes[n].meshes[m].primitives[p].struct_vertex_buffer;
                    int smallVertSize = model.nodes[n].meshes[m].primitives[p].buffer_size;
                    memcpy((char*)buff+vertBufferOffset,model.nodes[n].meshes[m].primitives[p].struct_vertex_buffer,smallVertSize*sizeof(Vertex_t));
                    vertBufferOffset += smallVertSize;
                    offsetList[offsetIndex] = vertBufferOffset;
                    offsetIndex += 1;
                    //model.nodes[n].meshes[m].primitives[p].index_buffer
                } 
           }
        }
    } 
}

void getTransitionNodeData(Node_t *nodes, int numberOfNodes){//used to hold on to the node values before transition for interp
    for(int i =0;i<numberOfNodes;i++){
        nodes[i].transitionScale = nodes[i].scale;
        nodes[i].transitionTranslation = nodes[i].translation;
        nodes[i].transitionRotation = nodes[i].rotation;
    }
}
void EaseToHome(Node_t *nodes, int numberOfNodes,float animationTime,float transitionTime){
    float interpolationTime = (animationTime - 0)/(transitionTime - 0);
    for(int i =0;i<numberOfNodes;i++){
        Vector3 lerpTranslate = lerpVec3(nodes[i].transitionTranslation,nodes[i].homeTranslation,interpolationTime);
        Vector4 lerpRotation = slerpQuaternion(nodes[i].transitionRotation,nodes[i].homeRotation,interpolationTime);
        Vector3 lerpScale = lerpVec3(nodes[i].transitionScale,nodes[i].homeScale,interpolationTime);
        //Node_t testNode = nodes[i];
        nodes[i].translation = lerpTranslate;
        nodes[i].rotation = lerpRotation;
        nodes[i].scale = lerpScale;
    }
}
void EaseToAnimation(Animation_t animation, Node_t *nodes, int numberOfNodes,float animationTime,float transitionTime){
    float interpolationTime = (animationTime - 0)/(transitionTime - 0);
    for(int i = 0;i<animation.numChannels;i++){
        Channel_t channel = animation.channels[i];
        int nodeIndex = channel.nodeIndex;
        int transformType = channel.transform_type;
        Sampler_t sampler = channel.sampler;
        float samplerTest = sampler.sampler_time[0];
        switch(transformType){
            case cgltf_animation_path_type_translation:
                Vector3 lerpTranslate = lerpVec3(nodes[nodeIndex].transitionTranslation,sampler.translation[0],interpolationTime);
                nodes[nodeIndex].translation = lerpTranslate;
                break;
            case cgltf_animation_path_type_rotation:
                Vector4 lerpRotate = slerpQuaternion(nodes[nodeIndex].transitionRotation,sampler.rotation[0],interpolationTime);
                nodes[nodeIndex].rotation = lerpRotate;
                break;
            case cgltf_animation_path_type_scale:
                Vector3 lerpScale = lerpVec3(nodes[nodeIndex].transitionScale,sampler.scale[0],interpolationTime);
                nodes[nodeIndex].scale = lerpScale;
                break;
            default:
                break;
        }
    }
}
void playAnimation(Animation_t animation,float currentTime,Node_t *nodes){
    //Animation_t animation = model_list.models[1].animations[0];
    for(int i =0;i<animation.numChannels;i++){
        Channel_t channel = animation.channels[i];
        int transformType = channel.transform_type;
        int nodeIndex = channel.nodeIndex;
        Sampler_t sampler = channel.sampler;
        int numberOfKeyFrames = sampler.num_sampler_time;
        for(int t=0;t<numberOfKeyFrames;t++){
            if(sampler.sampler_time[t] ==  currentTime){
                switch(transformType){
                    case cgltf_animation_path_type_translation:
                        nodes[nodeIndex].translation = sampler.translation[t];
                        break;
                    case cgltf_animation_path_type_rotation:
                        nodes[nodeIndex].rotation = sampler.rotation[t];
                        break;
                    case cgltf_animation_path_type_scale:
                        nodes[nodeIndex].scale = sampler.scale[t];
                        break;
                    default:
                        break;
                }
                break;
            }
            else if(sampler.sampler_time[t] > currentTime){
                float smallerTime = sampler.sampler_time[t-1];
                float biggerTime = sampler.sampler_time[t];
                float interpolationTime = (currentTime - smallerTime)/(biggerTime-smallerTime);
                if(interpolationTime>1){
                    printf("breakpoint");
                }
                printf("debug");
                //we need to collect the two sampler times as well as the data points
                //and interpolate them depending upon the the time
                switch(transformType){
                    case cgltf_animation_path_type_translation:
                        nodes[nodeIndex].translation = lerpVec3(sampler.translation[t-1],sampler.translation[t],interpolationTime);
                        break;
                    case cgltf_animation_path_type_rotation:
                        nodes[nodeIndex].rotation = slerpQuaternion(sampler.rotation[t-1],sampler.rotation[t],interpolationTime);
                        break;
                    case cgltf_animation_path_type_scale:
                        nodes[nodeIndex].scale = lerpVec3(sampler.scale[t-1],sampler.scale[t],interpolationTime);
                        break;
                    default:
                        break;
                }
                break;
            }
        }
    }
    
}
AABB calcAABBFromVertexBuffer(Vertex_t *vertBuffer, int size){
    AABB aabb;
    float maxX,maxY,maxZ,minX,minY,minZ;
    if(size>0){
        maxX = vertBuffer[0].position.x;
        minX = vertBuffer[0].position.x;
        maxY = vertBuffer[0].position.y;
        minY = vertBuffer[0].position.y;
        maxZ = vertBuffer[0].position.z;
        minZ = vertBuffer[0].position.z;
    }

    for(int i = 0;i<size;i++){
        if(vertBuffer[i].position.x<minX){
            minX = vertBuffer[i].position.x;
        }
        if(vertBuffer[i].position.x>maxX){
            maxX = vertBuffer[i].position.x;
        }
        if(vertBuffer[i].position.y<minY){
            minY = vertBuffer[i].position.y;
        }
        if(vertBuffer[i].position.y>maxY){
            maxY = vertBuffer[i].position.y;
        }
        if(vertBuffer[i].position.z<minZ){
            minZ = vertBuffer[i].position.z;
        }
        if(vertBuffer[i].position.z>maxZ){
            maxZ = vertBuffer[i].position.z;
        }
    }
    aabb.xmax = maxX;
    aabb.xmin = minX;
    aabb.ymax = maxY;
    aabb.ymin = minY;
    aabb.zmax = maxZ;
    aabb.zmin = minZ;

    return aabb;

}

void calculateChildRecursive(Node_t *nodes, int nodeIndex){
    Node_t *node = &nodes[nodeIndex];
    int parentIndex = node->parentIndex;
    int numberChildTest = node->numChildren;
    int indexTest = node->index;
    Mat4 parentWorldTransform; 
    if(node->hasParent){
        parentWorldTransform = nodes[parentIndex].worldTransformMatrix;
    }
    else{
        parentWorldTransform = identityMat4();
    }
    node->worldTransformMatrix = mulMat4(parentWorldTransform,node->transformMatrix);
    if(node->numChildren==0){
        return;
    }
    for(int i = 0;i<node->numChildren;i++){
        int childIndex = node->childIndices[i];
        calculateChildRecursive(nodes,childIndex);
    }
}

void recalculateSkinningMatrix(Node_t *nodes,int numberOfNodes,skinMatrix_t *skinMatrix){
    //for(int i =0;i<model->numberOfNodes;i++){
    //    int numchild = model->nodes[i].numChildren;
    //    for(int t = 0;t<numchild;t++){
    //        int index = model->nodes[i].childIndices[t];
    //        printf("break");
    //    }
    //}
    for(int i =0; i<numberOfNodes;i++){
        if(nodes[i].hasParent == false){
            int numberOfChildren = nodes[i].numChildren;
            for(int c = 0;c<numberOfChildren;c++){
                int childIndex = nodes[i].childIndices[c];
                calculateChildRecursive(nodes,childIndex);
                printf("test");
            }
        }
    }
    for(int i =0;i<numberOfNodes;i++){
        skinMatrix[i].skinMatrix = mulMat4(nodes[i].worldTransformMatrix,nodes[i].inverseJointMatrix);
        Node_t node = nodes[i];
        if(strcmp(nodes[i].name,"b_RightLeg01_019") == 0){
            Mat4 skin = skinMatrix[i].skinMatrix;
            printf("pause");
        //printf("pause"); 
        }
        
        printf("pause");
    }
}
void recalculateLocalTransformMatrix(Node_t *nodes,int numberOfNodes){
    for(int i = 0;i<numberOfNodes;i++){
        Vector3 translation = nodes[i].translation;
        Vector3 scale = nodes[i].scale;
        Vector4 rotation = nodes[i].rotation;
        Mat4 transformTest =  nodes[i].transformMatrix;
        nodes[i].transformMatrix = TRSMat4(translation,rotation,scale);
        Mat4 transformTest2 = nodes[i].transformMatrix;
        printf("debug");
    }
}
void fillModelBuffers(Model_t *model,ModelBuffers_t *modelBuffers){// we should use memory arenas here
    size_t v_size;
    modelBuffers->numOffsets=0;
    getNumberOfVertsInModel(*model,&v_size,&modelBuffers->numOffsets);
    modelBuffers->combinedVertBuffer = malloc(v_size*sizeof(Vertex_t));
    assert(modelBuffers->combinedVertBuffer != NULL && "combined vert buffer failed to allocate");
    modelBuffers->vertexOffsets = malloc(modelBuffers->numOffsets*sizeof(int));
    assert(modelBuffers->vertexOffsets != NULL && "failed to allocate");
    fillCombinedVertBuffer(modelBuffers->combinedVertBuffer,v_size,*model,modelBuffers->vertexOffsets);

    //sg_buffer vbuffer = makeVertexBuffer_Vertex_t(modelBuffers->combinedVertBuffer,v_size);

    size_t i_size;
    getNumberOfIndicesInModel(*model,&i_size); 
    //size_t i_size = model.nodes[0].meshes[0].primitives[0].index_buffer_size;
    modelBuffers->combinedIndexBuffer = malloc(i_size*sizeof(uint16_t));
    assert(modelBuffers->combinedIndexBuffer!=NULL && "Combined index buffer failed to allocate");
    modelBuffers->indexOffsets = malloc(modelBuffers->numOffsets*sizeof(int));
    assert(modelBuffers->indexOffsets != NULL && "failed to allocate");
    fillCombinedIndexBuffer(modelBuffers->combinedIndexBuffer,i_size,*model,modelBuffers->indexOffsets);
    //sg_buffer ibuffer = makeIndexBuffer(modelBuffers->combinedIndexBuffer,i_size);

    //modelBuffers->vbuffer = vbuffer;
    modelBuffers->vbuffer_size = v_size;
    //modelBuffers->ibuffer = ibuffer;
    modelBuffers->ibuffer_size = i_size;
}
/*
void recalculateSkinningMatrix(Model_t *model,skinMatrix_t *skinMatrix){
    for(int i = 0;i<model->numberOfNodes;i++){
        int numberOfChildren = model->nodes[i].numChildren;
        Mat4 parentWorldTransform = model->nodes[i].worldTransformMatrix;
        for(int c = 0;c<numberOfChildren;c++){
            int childIndex = model->nodes[i].childIndices[c];
            model->nodes[childIndex].worldTransformMatrix = mulMat4(model->nodes[childIndex].transformMatrix,parentWorldTransform);
        }
    }
    for(int i = 0;i<model->numberOfNodes;i++){
        skinMatrix[i].skinMatrix = mulMat4(model->nodes[i].worldTransformMatrix,model->nodes[i].inverseJointMatrix);
    }
    
}*/
const Cube cube_model_data = {
    //Vertices
    {
        1.00, -1.00, -1.00,  -0.00, -1.00, 0.00 ,
        -1.00, -1.00, -1.00,  -0.00, -1.00, 0.00 ,
        1.00, -1.00, 1.00,  -0.00, -1.00, 0.00 ,
        1.00, 1.00, -1.00,  0.00, 1.00, -0.00 ,
        1.00, 1.00, 1.00,  0.00, 1.00, -0.00 ,
        -1.00, 1.00, -1.00,  0.00, 1.00, -0.00 ,
        1.00, -1.00, -1.00,  1.00, -0.00, -0.00 ,
        1.00, -1.00, 1.00,  1.00, -0.00, -0.00 ,
        1.00, 1.00, -1.00,  1.00, -0.00, -0.00 ,
        1.00, -1.00, 1.00,  -0.00, -0.00, 1.00 ,
        -1.00, -1.00, 1.00,  -0.00, -0.00, 1.00 ,
        1.00, 1.00, 1.00,  -0.00, -0.00, 1.00 ,
        -1.00, -1.00, -1.00,  -1.00, -0.00, -0.00 ,
        -1.00, 1.00, -1.00,  -1.00, -0.00, -0.00 ,
        -1.00, -1.00, 1.00,  -1.00, -0.00, -0.00 ,
        1.00, 1.00, -1.00,  0.00, 0.00, -1.00 ,
        -1.00, 1.00, -1.00,  0.00, 0.00, -1.00 ,
        1.00, -1.00, -1.00,  0.00, 0.00, -1.00 ,
        -1.00, -1.00, -1.00,  0.00, -1.00, 0.00 ,
        -1.00, -1.00, 1.00,  0.00, -1.00, 0.00 ,
        1.00, -1.00, 1.00,  0.00, -1.00, 0.00 ,
        1.00, 1.00, 1.00,  0.00, 1.00, 0.00 ,
        -1.00, 1.00, 1.00,  0.00, 1.00, 0.00 ,
        -1.00, 1.00, -1.00,  0.00, 1.00, 0.00 ,
        1.00, -1.00, 1.00,  1.00, 0.00, 0.00 ,
        1.00, 1.00, 1.00,  1.00, 0.00, 0.00 ,
        1.00, 1.00, -1.00,  1.00, 0.00, 0.00 ,
        -1.00, -1.00, 1.00,  -0.00, 0.00, 1.00 ,
        -1.00, 1.00, 1.00,  -0.00, 0.00, 1.00 ,
        1.00, 1.00, 1.00,  -0.00, 0.00, 1.00 ,
        -1.00, 1.00, -1.00,  -1.00, -0.00, -0.00 ,
        -1.00, 1.00, 1.00,  -1.00, -0.00, -0.00 ,
        -1.00, -1.00, 1.00,  -1.00, -0.00, -0.00 ,
        -1.00, 1.00, -1.00,  0.00, 0.00, -1.00 ,
        -1.00, -1.00, -1.00,  0.00, 0.00, -1.00 ,
        1.00, -1.00, -1.00,  0.00, 0.00, -1.00
    },
    {//Indices
        0,1,2,3,
        4,5,6,7,8,9,10,11,
        12,13,14,15,16,17,
        18,19,20,21,22,23,
        24,25,26,27,28,29,
        30,31,32,33,34,35
    }

};



MeshData meshList[] = 
{
    {
        cube_model_data.vertices,
        216,
        cube_model_data.indices,
        36
    }
};
AABB meshAABB[] = {
    {-1.0f,1.0f,-1.0f,1.0f,-1.0f,1.0f}
};

void cleanUpModelData(){
    free(modelArena.buffer);
    /*
    int numberOfModels = model_list.numberOfModels;
    //Free the primitives
    for(int m = 0; m <numberOfModels;m++){
        int numberOfNodes = model_list.models[m].numberOfNodes;
        Node_t *nodes = model_list.models[m].nodes;
        for(int n = 0;n<numberOfNodes;n++){
            int numberOfMeshes = nodes[n].numberOfMeshes;
            Mesh_t *meshes = nodes[n].meshes;
            for(int me=0; me<numberOfMeshes;me++){
                int numberOfPrimitives = meshes[me].numberOfPrimitives;
                Primitive_t *primitives = meshes[me].primitives;
                for(int p = 0;p<numberOfPrimitives;p++){
                    if(primitives[p].combined_vert_buffer!=NULL){
                        free(primitives[p].combined_vert_buffer);
                    }
                    if(primitives[p].index_buffer!=NULL){
                        free(primitives[p].index_buffer);
                    }
                    if(primitives[p].normal_buffer!=NULL){
                        free(primitives[p].normal_buffer);
                    }
                    if(primitives[p].vertex_buffer!=NULL){
                        free(primitives[p].vertex_buffer);
                    }

                }
                if(primitives!=NULL){
                    free(primitives);
                }
            }
            if(meshes!=NULL){
                free(meshes); 
            }
            
        }
        if(nodes != NULL){
            free(nodes);
        }
    }
    free(model_list.models);
    */
    //int numberOfMeshes = node.numberOfMeshes;

    
    /*for(int i =0;i<numberOfMeshes;i++){
        Mesh_t *mesh = &node.meshes[i];

        for(int t = 0;t<mesh->numberOfPrimitives;t++){
            free(mesh->primitives[t].vertex_buffer);
            free(mesh->primitives[t].normal_buffer);
            free(mesh->primitives[t].index_buffer);
        }
        
    }*/
}

/*AABB meshAABB[] = {
    {
        -1.0f,
        1.0f,
        -1.0f,
        1.0f,
        -1.0f,
        1.0f
    }
};*/









#endif