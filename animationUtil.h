//this file is a helper utility helper file for
//animation models that use gltf file format

/*
THIS IS YANKING A LOT OF THE ANIMATION FUNCTIONALITY THAT I HAD IN MY PREVIOUS
MODELDATA GLTF MODEL IMPORTER

I FIGURED THAT IT WOULD BE BETTER TO SEPARATE OUT ALOT OF THE ANIMATION
SPECIFIC IMPLEMENTATION INTO ITS OWN FILE BECAUSE IT WILL NEED MORE FUNCTIONALITY WHEN
WE ARE DOING THINGS LIKE BLENDING AND DIFFERENT EASING METHODS
*/


#ifndef ANIMATION_UTIL_H
#define ANIMATION_UTIL_H
#include "mathUtil.h"


typedef struct Anim_Sampler_t{
    float *sampler_time;
    int num_sampler_time;
    Vector4 *rotation;
    Vector3 *translation;
    Vector3 *scale;
    int sampler_transform_count;
    int interpolation;
}Anim_Sampler_t;
typedef struct Anim_Channel_t{
    Anim_Sampler_t sampler;
    int nodeIndex;
    int transform_type;
}Anim_Channel_t;
typedef struct Anim_Animation_t{
    char name[100];
    Anim_Channel_t *channels;
    int numChannels;
    float animationDuration;
}Anim_Animation_t;
typedef struct{

    Anim_Animation_t *animations;
    int num_animations;
    bool playingAnimation;
    bool endingAnimation;
    bool startAnimation;
    int activeAnimationIndex;
    uint64_t animationStartTime;
    uint64_t currentAnimationTime;
    float transitionTime;

}Anim_State_t;


typedef struct Anim_Node_t{
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
}Anim_Node_t;





void Anim_EaseToHome(Anim_Node_t *nodes, int numberOfNodes, float animationTime, float transitionTime);
void Anim_EaseToAnimation(Anim_Animation_t animation, Anim_Node_t *nodes, int numberOfNodes, float animationTime, float transitionTime);
void Anim_playAnimation(Anim_Animation_t animation, float currentTime, Anim_Node_t *nodes);

#ifdef ANIMATION_UTIL_IMPLEMENTATION

void Anim_EaseToHome(Anim_Node_t *nodes, int numberOfNodes,float animationTime,float transitionTime){
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

void Anim_EaseToAnimation(Anim_Animation_t animation, Anim_Node_t *nodes, int numberOfNodes,float animationTime,float transitionTime){
    float interpolationTime = (animationTime - 0)/(transitionTime - 0);
    for(int i = 0;i<animation.numChannels;i++){
        Anim_Channel_t channel = animation.channels[i];
        int nodeIndex = channel.nodeIndex;
        int transformType = channel.transform_type;
        Anim_Sampler_t sampler = channel.sampler;
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

void Anim_playAnimation(Anim_Animation_t animation,float currentTime,Anim_Node_t *nodes){
    //Animation_t animation = model_list.models[1].animations[0];
    for(int i =0;i<animation.numChannels;i++){
        Anim_Channel_t channel = animation.channels[i];
        int transformType = channel.transform_type;
        int nodeIndex = channel.nodeIndex;
        Anim_Sampler_t sampler = channel.sampler;
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
                    //printf("breakpoint");
                }
                //printf("debug");
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



#endif //impl endif


#endif// header guard endif