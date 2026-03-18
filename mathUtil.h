#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <math.h>
#ifdef _MSC_VER
#pragma warning(disable:4201)
#endif


//////////////////
//////NOTE

/*
    m[column][row]
    
        Column →
      0        1        2        3
    +--------+--------+--------+--------+
 0  | 0,0    | 1,0    | 2,0    | 3,0    |
    +--------+--------+--------+--------+
 1  | 0,1    | 1,1    | 2,1    | 3,1    |
    +--------+--------+--------+--------+
 2  | 0,2    | 1,2    | 2,2    | 3,2    |
    +--------+--------+--------+--------+
 3  | 0,3    | 1,3    | 2,3    | 3,3    |
    +--------+--------+--------+--------+

      ↑
     Row


*/

typedef union{
    struct{
        float x,y;
    };
    float Elements[2];
}Vector2;
//by making this a union, we can access the same memory location with different names
//x will access the same space as r.
typedef union{
    struct{
        float x,y,z;
    };
    struct{
        float r,g,b;
    };
    Vector2 xy;
    float Elements[3];
}Vector3;
typedef union{
    struct{
        float x,y,z,w;
    };
    struct{
        float r,g,b,a;
    };
    Vector3 xyz;
    float Elements[4];
}Vector4;
typedef union{
    struct{
        unsigned int x,y,z,w;
    };
    struct{
        unsigned int r,g,b,a;
    };
    unsigned int Elements[4];
}Vector4_uint;
typedef union{
    float Elements[4][4];
    float Elements_16[16];
}Mat4;
typedef struct{
    float Elements[3][3];
}Mat3;
typedef struct{
    float Elements[2][2];
}Mat2;

const Vector3 Vec3Up ={0.0f,1.0f,0.0f};
float magVec3(Vector3 vector){
    return sqrtf(vector.x*vector.x+vector.y*vector.y+vector.z*vector.z);
}
Vector3 crossVec3(Vector3 left ,Vector3 right){
    Vector3 cross = {0,0,0};
    cross.x = (left.y*right.z) - (left.z*right.y);
    cross.y = (left.z*right.x) - (left.x*right.z);
    cross.z = (left.x*right.y) - (left.y*right.x);
    return cross;
}
Vector3 normalizeVec3(Vector3 src){
    //float sum  = src.x*src.x + src.y*src.y + src.z*src.z;
    float mag = sqrtf((float)(src.x*src.x + src.y*src.y + src.z*src.z));
    
    Vector3 norm;
    if(mag == 0){
        norm.x = 0.0f;
        norm.y = 0.0f;
        norm.z = 0.0f;
    }
    else{
        norm.x = src.x/mag;
        norm.y = src.y/mag;
        norm.z = src.z/mag;  
    }
    return norm;
}

Vector3 subtractVec3(Vector3 vec1,Vector3 vec2){
    Vector3 result = {0.0f,0.0f,0.0f};
    result.x = vec1.x-vec2.x;
    result.y = vec1.y-vec2.y;
    result.z = vec1.z-vec2.z;
    return result;
}
Vector3 addVec3(Vector3 vec1, Vector3 vec2){
    Vector3 result = {0.0f,0.0f,0.0f};
    result.x = vec1.x + vec2.x;
    result.y = vec1.y + vec2.y;
    result.z = vec1.z + vec2.z;
    return result;
}
Vector3 mulVec3Float(Vector3 vec,float val){
    Vector3 result;
    result.x = vec.x * val;
    result.y = vec.y * val;
    result.z = vec.z * val;
    return result;
}
float dotVec3(Vector3 vec1,Vector3 vec2){
    return(vec1.x*vec2.x+vec1.y*vec2.y+vec1.z*vec2.z);
}
Vector3 lerpVec3(Vector3 A, Vector3 B, float t){
    //Vec3 result = A * (1.0f - t) + B * t;
    Vector3 result;
    result.x = A.x*(1.0f-t) + B.x*t;
    result.y = A.y*(1.0f-t) + B.y*t;
    result.z = A.z*(1.0f-t) + B.z*t;
    
    return result;
}
float distanceVec3(Vector3 A,Vector3 B){
    return sqrtf(powf(A.x-B.x,2) + powf(A.y-B.y,2) + powf(A.z-B.z,2));

}
//-----------------------------------------------------
//VECTOR4
Vector4 mulVec4Float(Vector4 vec, float val){
    Vector4 result;
    result.x = vec.x*val;
    result.y = vec.y*val;
    result.z = vec.z*val;
    result.w = vec.w*val;
    return result;
}
Vector4 addVec4(Vector4 vec1, Vector4 vec2){
    Vector4 result = {0.0f,0.0f,0.0f,0.0f};
    result.x = vec1.x + vec2.x;
    result.y = vec1.y + vec2.y;
    result.z = vec1.z + vec2.z;
    result.w = vec1.w + vec2.w;
    return result;
}
Vector4 mulMat4Vec4(Mat4 mat,Vector4 vec){
    Vector4 result;

    result.x = mat.Elements[0][0]*vec.Elements[0] + mat.Elements[1][0]*vec.Elements[1]+mat.Elements[2][0]*vec.Elements[2]+mat.Elements[3][0]*vec.Elements[3];
    result.y = mat.Elements[0][1]*vec.Elements[0] + mat.Elements[1][1]*vec.Elements[1]+mat.Elements[2][1]*vec.Elements[2]+mat.Elements[3][1]*vec.Elements[3];
    result.z = mat.Elements[0][2]*vec.Elements[0] + mat.Elements[1][2]*vec.Elements[1]+mat.Elements[2][2]*vec.Elements[2]+mat.Elements[3][2]*vec.Elements[3];
    result.w = mat.Elements[0][3]*vec.Elements[0] + mat.Elements[1][3]*vec.Elements[1]+mat.Elements[2][3]*vec.Elements[2]+mat.Elements[3][3]*vec.Elements[3];
    return result;
}
Vector4 lerpVec4(Vector4 A, Vector4 B, float t){
    //Vec3 result = A * (1.0f - t) + B * t;
    Vector4 result;
    result.x = A.x*(1.0f-t) + B.x*t;
    result.y = A.y*(1.0f-t) + B.y*t;
    result.z = A.z*(1.0f-t) + B.z*t;
    result.w = A.w*(1.0f-t) + B.w*t;
    
    return result;
}
float dotVec4(Vector4 vec1, Vector4 vec2){
    return(vec1.x*vec2.x+vec1.y*vec2.y+vec1.z*vec2.z+vec1.w*vec2.w);
}
Vector4 normalizeVec4(Vector4 src){
    float mag = sqrtf((float)(src.x*src.x + src.y*src.y + src.z*src.z +src.w*src.w));
    
    Vector4 norm;
    if(mag == 0){
        norm.x = 0.0f;
        norm.y = 0.0f;
        norm.z = 0.0f;
        norm.w = 0.0f;
    }
    else{
        norm.x = src.x/mag;
        norm.y = src.y/mag;
        norm.z = src.z/mag;  
        norm.w = src.w/mag;
    }
    return norm;
}
//-----------------------------------------------------
//This is the beginning of the matrix section
const Mat4 identityMat4Const = {
    { 
        {1.0f,0.0f,0.0f,0.0f},
        {0.0,1.0f,0.0f,0.0f},
        {0.0f,0.0f,1.0f,0.0f},
        {0.0f,0.0f,0.0f,1.0f}
    }
};

Mat4 identityMat4(){
    Mat4 identity;
    for(int x=0;x<4;x++){
        for(int y=0;y<4;y++){
            if(x==y){
                identity.Elements[x][y] = 1.0f;
            }
            else{
                identity.Elements[x][y] = 0.0f;  
            }
        }
    }
    return identity;
}
Mat4 LookAt_RH(Vector3 Right,Vector3 U,Vector3 D,Vector3 P){ //THIS IS NOT RIGHT FOR SOME REASON???????
    Mat4 result;
    result.Elements[0][0] = Right.x;
    result.Elements[1][0] = Right.y;
    result.Elements[2][0] = Right.z;
    result.Elements[3][0] =  -dotVec3(Right,P);

    result.Elements[0][1] = U.x;
    result.Elements[1][1] = U.y;
    result.Elements[2][1] = U.z;
    result.Elements[3][1] = -dotVec3(U,P);

    result.Elements[0][2] = -D.x;
    result.Elements[1][2] = -D.y;
    result.Elements[2][2] = -D.z;
    result.Elements[3][2] = dotVec3(D,P);
    
    result.Elements[0][3] = 0.0f;
    result.Elements[1][3] = 0.0f;
    result.Elements[2][3] = 0.0f;
    result.Elements[3][3] = 1.0f;

    return result;
}
Mat4 LookAt_RH_Version2(Vector3 Eye, Vector3 Center, Vector3 Up){
    Vector3 F = normalizeVec3(subtractVec3(Center, Eye));
    Vector3 S = normalizeVec3(crossVec3(F, Up));
    Vector3 U = crossVec3(S, F);

    Mat4 Result;

    Result.Elements[0][0] = S.x;
    Result.Elements[0][1] = U.x;
    Result.Elements[0][2] = -F.x;
    Result.Elements[0][3] = 0.0f;

    Result.Elements[1][0] = S.y;
    Result.Elements[1][1] = U.y;
    Result.Elements[1][2] = -F.y;
    Result.Elements[1][3] = 0.0f;

    Result.Elements[2][0] = S.z;
    Result.Elements[2][1] = U.z;
    Result.Elements[2][2] = -F.z;
    Result.Elements[2][3] = 0.0f;

    Result.Elements[3][0] = -dotVec3(S, Eye);
    Result.Elements[3][1] = -dotVec3(U, Eye);
    Result.Elements[3][2] = dotVec3(F, Eye);
    Result.Elements[3][3] = 1.0f;

    return Result;
}

Mat4 mulMat4(Mat4 left,Mat4 right){
    Mat4 result;
    result.Elements[0][0] = left.Elements[0][0]*right.Elements[0][0] + left.Elements[1][0]*right.Elements[0][1]+ left.Elements[2][0]*right.Elements[0][2]+left.Elements[3][0]*right.Elements[0][3];
    result.Elements[1][0] = left.Elements[0][0]*right.Elements[1][0] + left.Elements[1][0]*right.Elements[1][1]+ left.Elements[2][0]*right.Elements[1][2]+left.Elements[3][0]*right.Elements[1][3];
    result.Elements[2][0] = left.Elements[0][0]*right.Elements[2][0] + left.Elements[1][0]*right.Elements[2][1]+ left.Elements[2][0]*right.Elements[2][2]+left.Elements[3][0]*right.Elements[2][3];
    result.Elements[3][0] = left.Elements[0][0]*right.Elements[3][0] + left.Elements[1][0]*right.Elements[3][1]+ left.Elements[2][0]*right.Elements[3][2]+left.Elements[3][0]*right.Elements[3][3];

    result.Elements[0][1] = left.Elements[0][1]*right.Elements[0][0] + left.Elements[1][1]*right.Elements[0][1]+ left.Elements[2][1]*right.Elements[0][2]+left.Elements[3][1]*right.Elements[0][3];
    result.Elements[1][1] = left.Elements[0][1]*right.Elements[1][0] + left.Elements[1][1]*right.Elements[1][1]+ left.Elements[2][1]*right.Elements[1][2]+left.Elements[3][1]*right.Elements[1][3];
    result.Elements[2][1] = left.Elements[0][1]*right.Elements[2][0] + left.Elements[1][1]*right.Elements[2][1]+ left.Elements[2][1]*right.Elements[2][2]+left.Elements[3][1]*right.Elements[2][3];
    result.Elements[3][1] = left.Elements[0][1]*right.Elements[3][0] + left.Elements[1][1]*right.Elements[3][1]+ left.Elements[2][1]*right.Elements[3][2]+left.Elements[3][1]*right.Elements[3][3];

    result.Elements[0][2] = left.Elements[0][2]*right.Elements[0][0] + left.Elements[1][2]*right.Elements[0][1]+ left.Elements[2][2]*right.Elements[0][2]+left.Elements[3][2]*right.Elements[0][3];
    result.Elements[1][2] = left.Elements[0][2]*right.Elements[1][0] + left.Elements[1][2]*right.Elements[1][1]+ left.Elements[2][2]*right.Elements[1][2]+left.Elements[3][2]*right.Elements[1][3];
    result.Elements[2][2] = left.Elements[0][2]*right.Elements[2][0] + left.Elements[1][2]*right.Elements[2][1]+ left.Elements[2][2]*right.Elements[2][2]+left.Elements[3][2]*right.Elements[2][3];
    result.Elements[3][2] = left.Elements[0][2]*right.Elements[3][0] + left.Elements[1][2]*right.Elements[3][1]+ left.Elements[2][2]*right.Elements[3][2]+left.Elements[3][2]*right.Elements[3][3];

    result.Elements[0][3] = left.Elements[0][3]*right.Elements[0][0] + left.Elements[1][3]*right.Elements[0][1]+ left.Elements[2][3]*right.Elements[0][2]+left.Elements[3][3]*right.Elements[0][3];
    result.Elements[1][3] = left.Elements[0][3]*right.Elements[1][0] + left.Elements[1][3]*right.Elements[1][1]+ left.Elements[2][3]*right.Elements[1][2]+left.Elements[3][3]*right.Elements[1][3];
    result.Elements[2][3] = left.Elements[0][3]*right.Elements[2][0] + left.Elements[1][3]*right.Elements[2][1]+ left.Elements[2][3]*right.Elements[2][2]+left.Elements[3][3]*right.Elements[2][3];
    result.Elements[3][3] = left.Elements[0][3]*right.Elements[3][0] + left.Elements[1][3]*right.Elements[3][1]+ left.Elements[2][3]*right.Elements[3][2]+left.Elements[3][3]*right.Elements[3][3];
    return result;
}   
Mat4 translateMat4(Vector3 translateVec3){
    Mat4 translateMat;
    translateMat = identityMat4();
    translateMat.Elements[3][0] = translateVec3.x;
    translateMat.Elements[3][1] = translateVec3.y;
    translateMat.Elements[3][2] = translateVec3.z;
    return translateMat;
}
Mat4 transposeMat4(Mat4 mat){
    Mat4 result = mat;
    for(int x=0;x<4;x++){
        for(int y=0;y<4;y++){
            result.Elements[x][y] = mat.Elements[y][x];
        }
    }
    return result;
}
//////////////////
//////NOTE

/*
    m[column][row]
    
        Column →
      0        1        2        3
    +--------+--------+--------+--------+
 0  | 0,0    | 1,0    | 2,0    | 3,0    |
    +--------+--------+--------+--------+
 1  | 0,1    | 1,1    | 2,1    | 3,1    |
    +--------+--------+--------+--------+
 2  | 0,2    | 1,2    | 2,2    | 3,2    |
    +--------+--------+--------+--------+
 3  | 0,3    | 1,3    | 2,3    | 3,3    |
    +--------+--------+--------+--------+

      ↑
     Row
*/
Mat4 orthographicMat4(float left, float right, float bottom, float top, float near_plane, float far_plane){
    Mat4 orthoMat;
    /*
            0        1        2        3
        +--------+--------+--------+--------+
    0  | 0,0    | 1,0    | 2,0    | 3,0    |
        +--------+--------+--------+--------+
    */
    orthoMat.Elements[0][0] = 2.0f/(right-left);
    orthoMat.Elements[1][0] = 0.0f;
    orthoMat.Elements[2][0] = 0.0f;
    orthoMat.Elements[3][0] = -1.0f*((right+left)/(right-left));
    /*
        +--------+--------+--------+--------+
    1  | 0,1    | 1,1    | 2,1    | 3,1    |
        +--------+--------+--------+--------+
    */
    orthoMat.Elements[0][1] = 0.0f;
    orthoMat.Elements[1][1] = 2.0f/(top-bottom);
    orthoMat.Elements[2][1] = 0.0f;
    orthoMat.Elements[3][1] = -1.0f*((top+bottom)/(top-bottom));
    /*
       +--------+--------+--------+--------+
    2  | 0,2    | 1,2    | 2,2    | 3,2    |
       +--------+--------+--------+--------+
    */
    orthoMat.Elements[0][2] = 0.0f;
    orthoMat.Elements[1][2] = 0.0f;
    orthoMat.Elements[2][2] = 2.0f/(far_plane-near_plane);
    orthoMat.Elements[3][2] = -1.0f*((far_plane+near_plane)/(far_plane-near_plane));
    /*
        +--------+--------+--------+--------+
    3  | 0,3    | 1,3    | 2,3    | 3,3    |
        +--------+--------+--------+--------+
    */
    orthoMat.Elements[0][3] = 0.0f;
    orthoMat.Elements[1][3] = 0.0f;
    orthoMat.Elements[2][3] = 0.0f;
    orthoMat.Elements[3][3] = 1.0f;

    return orthoMat;

}
Mat3 Mat4ToMat3(Mat4 mat){
    Mat3 result;
    for(int y=0;y<3;y++){
        for(int x=0;x<3;x++){
            result.Elements[x][y] = mat.Elements[x][y];
        }
    }
    return result;
}
Mat4 Mat3ToMat4(Mat3 mat){
    Mat4 result;
    for(int y=0;y<3;y++){
        for(int x=0;x<3;x++){
            result.Elements[x][y] = mat.Elements[x][y];
        }
    }
    result.Elements[3][0] = 0;
    result.Elements[3][1] = 0;
    result.Elements[3][2] = 0;
    result.Elements[3][3] = 1;

    result.Elements[0][3] = 0;
    result.Elements[1][3] = 0;
    result.Elements[2][3] = 0;

    

    return result;
}
float determinantMat3(Mat3 mat){//UNTESTED
    float determinant=0;
    //A
    determinant += mat.Elements[0][0] * (mat.Elements[1][1]*mat.Elements[2][2]-
    mat.Elements[2][1]*mat.Elements[1][2]);
    //B
    determinant -= mat.Elements[1][0] * (mat.Elements[0][1]*mat.Elements[2][2]-
    mat.Elements[2][1]*mat.Elements[0][2]);
    //C
    determinant += mat.Elements[2][0] * (mat.Elements[0][1]*mat.Elements[1][2]-
    mat.Elements[1][1]*mat.Elements[0][2]);

    return determinant;

}
Mat4 rotateMat4(float angle,Vector3 axis){

    Mat4 result = identityMat4Const;
    
    Vector3 r = normalizeVec3(axis);
    float cosTheta = cosf(angle);
    float sinTheta = sinf(angle);
    float invCos = 1 - cosTheta;

    
    
    result.Elements[0][0] = cosTheta + (r.x * r.x * invCos);
    result.Elements[1][0] = (r.x*r.y*invCos) - (r.z*sinTheta);
    result.Elements[2][0] = (r.x*r.z*invCos) + (r.y*sinTheta);
    result.Elements[3][0] = 0;
    
    result.Elements[0][1] = (r.y*r.x*invCos) + (r.z*sinTheta);
    result.Elements[1][1] = cosTheta + (r.y*r.y*invCos);
    result.Elements[2][1] = (r.y*r.z*invCos) - (r.x*sinTheta);
    result.Elements[3][1] = 0;

    result.Elements[0][2] = (r.z*r.x*invCos) - (r.y*sinTheta);
    result.Elements[1][2] = (r.z*r.y*invCos) + (r.x*sinTheta);
    result.Elements[2][2] = cosTheta + (r.x*r.x*invCos);
    result.Elements[3][2] = 0;
    
    result.Elements[0][3] = 0;
    result.Elements[1][3] = 0;
    result.Elements[2][3] = 0;
    result.Elements[3][3] = 1;

    return result;

}
Mat4 rotateMat4Version2(float Angle,Vector3 Axis){
   
    Mat4 Result = identityMat4Const;

    Axis = normalizeVec3(Axis);

    float SinTheta = sinf(Angle);
    float CosTheta = cosf(Angle);
    float CosValue = 1.0f - CosTheta;

    Result.Elements[0][0] = (Axis.x * Axis.x * CosValue) + CosTheta;
    Result.Elements[0][1] = (Axis.x * Axis.y * CosValue) + (Axis.z * SinTheta);
    Result.Elements[0][2] = (Axis.x * Axis.z * CosValue) - (Axis.y * SinTheta);

    Result.Elements[1][0] = (Axis.y * Axis.x * CosValue) - (Axis.z * SinTheta);
    Result.Elements[1][1] = (Axis.y * Axis.y * CosValue) + CosTheta;
    Result.Elements[1][2] = (Axis.y * Axis.z * CosValue) + (Axis.x * SinTheta);

    Result.Elements[2][0] = (Axis.z * Axis.x * CosValue) + (Axis.y * SinTheta);
    Result.Elements[2][1] = (Axis.z * Axis.y * CosValue) - (Axis.x * SinTheta);
    Result.Elements[2][2] = (Axis.z * Axis.z * CosValue) + CosTheta;

    return Result;

}
/*
    [0][1][2][3]
     |  |  |  |
[0]-[a][b][c][d]
[1]-[e][f][g][h]
[2]-[i][j][k][l]
[3]-[m][n][o][p]
*/
float determinantMat4(Mat4 mat){
    float determinant=0;
    Mat3 detMat3;
    
    detMat3.Elements[0][0] = mat.Elements[1][1];
    detMat3.Elements[0][1] = mat.Elements[1][2];
    detMat3.Elements[0][2] = mat.Elements[1][3];
    detMat3.Elements[1][0] = mat.Elements[2][1];
    detMat3.Elements[1][1] = mat.Elements[2][2];
    detMat3.Elements[1][2] = mat.Elements[2][3];
    detMat3.Elements[2][0] = mat.Elements[3][1];
    detMat3.Elements[2][1] = mat.Elements[3][2];
    detMat3.Elements[2][2] = mat.Elements[3][3];

    determinant += mat.Elements[0][0]*determinantMat3(detMat3);

    detMat3.Elements[0][0] = mat.Elements[0][1];
    detMat3.Elements[0][1] = mat.Elements[0][2];
    detMat3.Elements[0][2] = mat.Elements[0][3];
    detMat3.Elements[1][0] = mat.Elements[2][1];
    detMat3.Elements[1][1] = mat.Elements[2][2];
    detMat3.Elements[1][2] = mat.Elements[2][3];
    detMat3.Elements[2][0] = mat.Elements[3][1];
    detMat3.Elements[2][1] = mat.Elements[3][2];
    detMat3.Elements[2][2] = mat.Elements[3][3];

    determinant -= mat.Elements[1][0] * determinantMat3(detMat3);

    detMat3.Elements[0][0] = mat.Elements[0][1];
    detMat3.Elements[0][1] = mat.Elements[0][2];
    detMat3.Elements[0][2] = mat.Elements[0][3];
    detMat3.Elements[1][0] = mat.Elements[1][1];
    detMat3.Elements[1][1] = mat.Elements[1][2];
    detMat3.Elements[1][2] = mat.Elements[1][3];
    detMat3.Elements[2][0] = mat.Elements[3][1];
    detMat3.Elements[2][1] = mat.Elements[3][2];
    detMat3.Elements[2][2] = mat.Elements[3][3];

    determinant += mat.Elements[2][0] * determinantMat3(detMat3);

    detMat3.Elements[0][0] = mat.Elements[0][1];
    detMat3.Elements[0][1] = mat.Elements[0][2];
    detMat3.Elements[0][2] = mat.Elements[0][3];
    detMat3.Elements[1][0] = mat.Elements[1][1];
    detMat3.Elements[1][1] = mat.Elements[1][2];
    detMat3.Elements[1][2] = mat.Elements[1][3];
    detMat3.Elements[2][0] = mat.Elements[2][1];
    detMat3.Elements[2][1] = mat.Elements[2][2];
    detMat3.Elements[2][2] = mat.Elements[2][3];

    determinant -= mat.Elements[3][0] * determinantMat3(detMat3);
    return determinant;
}
//INVERSION IMPLEMENTATION FROM https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
Mat4 inverseMat4(Mat4 mat){ 
    Mat4 result;
    result.Elements[0][0] = mat.Elements[1][1] * mat.Elements[2][2] * mat.Elements[3][3] -
                        mat.Elements[1][1] * mat.Elements[2][3] * mat.Elements[3][2] -
                        mat.Elements[2][1] * mat.Elements[1][2] * mat.Elements[3][3] +
                        mat.Elements[2][1] * mat.Elements[1][3] * mat.Elements[3][2] +
                        mat.Elements[3][1] * mat.Elements[1][2] * mat.Elements[2][3] -
                        mat.Elements[3][1] * mat.Elements[1][3] * mat.Elements[2][2];

    result.Elements[1][0] = -mat.Elements[1][0] * mat.Elements[2][2] * mat.Elements[3][3] +
                        mat.Elements[1][0] * mat.Elements[2][3] * mat.Elements[3][2] +
                        mat.Elements[2][0] * mat.Elements[1][2] * mat.Elements[3][3] -
                        mat.Elements[2][0] * mat.Elements[1][3] * mat.Elements[3][2] -
                        mat.Elements[3][0] * mat.Elements[1][2] * mat.Elements[2][3] +
                        mat.Elements[3][0] * mat.Elements[1][3] * mat.Elements[2][2];

    result.Elements[2][0] = mat.Elements[1][0] * mat.Elements[2][1] * mat.Elements[3][3] -
                        mat.Elements[1][0] * mat.Elements[2][3] * mat.Elements[3][1] -
                        mat.Elements[2][0] * mat.Elements[1][1] * mat.Elements[3][3] +
                        mat.Elements[2][0] * mat.Elements[1][3] * mat.Elements[3][1] +
                        mat.Elements[3][0] * mat.Elements[1][1] * mat.Elements[2][3] -
                        mat.Elements[3][0] * mat.Elements[1][3] * mat.Elements[2][1];

    result.Elements[3][0] = -mat.Elements[1][0] * mat.Elements[2][1] * mat.Elements[3][2] +
                        mat.Elements[1][0] * mat.Elements[2][2] * mat.Elements[3][1] +
                        mat.Elements[2][0] * mat.Elements[1][1] * mat.Elements[3][2] -
                        mat.Elements[2][0] * mat.Elements[1][2] * mat.Elements[3][1] -
                        mat.Elements[3][0] * mat.Elements[1][1] * mat.Elements[2][2] +
                        mat.Elements[3][0] * mat.Elements[1][2] * mat.Elements[2][1];

    result.Elements[0][1] = -mat.Elements[0][1] * mat.Elements[2][2] * mat.Elements[3][3] +
                        mat.Elements[0][1] * mat.Elements[2][3] * mat.Elements[3][2] +
                        mat.Elements[2][1] * mat.Elements[0][2] * mat.Elements[3][3] -
                        mat.Elements[2][1] * mat.Elements[0][3] * mat.Elements[3][2] -
                        mat.Elements[3][1] * mat.Elements[0][2] * mat.Elements[2][3] +
                        mat.Elements[3][1] * mat.Elements[0][3] * mat.Elements[2][2];

    result.Elements[1][1] = mat.Elements[0][0] * mat.Elements[2][2] * mat.Elements[3][3] -
                        mat.Elements[0][0] * mat.Elements[2][3] * mat.Elements[3][2] -
                        mat.Elements[2][0] * mat.Elements[0][2] * mat.Elements[3][3] +
                        mat.Elements[2][0] * mat.Elements[0][3] * mat.Elements[3][2] +
                        mat.Elements[3][0] * mat.Elements[0][2] * mat.Elements[2][3] -
                        mat.Elements[3][0] * mat.Elements[0][3] * mat.Elements[2][2];

    result.Elements[2][1] = -mat.Elements[0][0] * mat.Elements[2][1] * mat.Elements[3][3] +
                        mat.Elements[0][0] * mat.Elements[2][3] * mat.Elements[3][1] +
                        mat.Elements[2][0] * mat.Elements[0][1] * mat.Elements[3][3] -
                        mat.Elements[2][0] * mat.Elements[0][3] * mat.Elements[3][1] -
                        mat.Elements[3][0] * mat.Elements[0][1] * mat.Elements[2][3] +
                        mat.Elements[3][0] * mat.Elements[0][3] * mat.Elements[2][1];

    result.Elements[3][1] = mat.Elements[0][0] * mat.Elements[2][1] * mat.Elements[3][2] -
                        mat.Elements[0][0] * mat.Elements[2][2] * mat.Elements[3][1] -
                        mat.Elements[2][0] * mat.Elements[0][1] * mat.Elements[3][2] +
                        mat.Elements[2][0] * mat.Elements[0][2] * mat.Elements[3][1] +
                        mat.Elements[3][0] * mat.Elements[0][1] * mat.Elements[2][2] -
                        mat.Elements[3][0] * mat.Elements[0][2] * mat.Elements[2][1];

    result.Elements[0][2] = mat.Elements[0][1] * mat.Elements[1][2] * mat.Elements[3][3] -
                        mat.Elements[0][1] * mat.Elements[1][3] * mat.Elements[3][2] -
                        mat.Elements[1][1] * mat.Elements[0][2] * mat.Elements[3][3] +
                        mat.Elements[1][1] * mat.Elements[0][3] * mat.Elements[3][2] +
                        mat.Elements[3][1] * mat.Elements[0][2] * mat.Elements[1][3] -
                        mat.Elements[3][1] * mat.Elements[0][3] * mat.Elements[1][2];

    result.Elements[1][2] = -mat.Elements[0][0] * mat.Elements[1][2] * mat.Elements[3][3] +
                        mat.Elements[0][0] * mat.Elements[1][3] * mat.Elements[3][2] +
                        mat.Elements[1][0] * mat.Elements[0][2] * mat.Elements[3][3] -
                        mat.Elements[1][0] * mat.Elements[0][3] * mat.Elements[3][2] -
                        mat.Elements[3][0] * mat.Elements[0][2] * mat.Elements[1][3] +
                        mat.Elements[3][0] * mat.Elements[0][3] * mat.Elements[1][2];

    result.Elements[2][2] = mat.Elements[0][0] * mat.Elements[1][1] * mat.Elements[3][3] -
                        mat.Elements[0][0] * mat.Elements[1][3] * mat.Elements[3][1] -
                        mat.Elements[1][0] * mat.Elements[0][1] * mat.Elements[3][3] +
                        mat.Elements[1][0] * mat.Elements[0][3] * mat.Elements[3][1] +
                        mat.Elements[3][0] * mat.Elements[0][1] * mat.Elements[1][3] -
                        mat.Elements[3][0] * mat.Elements[0][3] * mat.Elements[1][1];

    result.Elements[3][2] = -mat.Elements[0][0] * mat.Elements[1][1] * mat.Elements[3][2] +
                        mat.Elements[0][0] * mat.Elements[1][2] * mat.Elements[3][1] +
                        mat.Elements[1][0] * mat.Elements[0][1] * mat.Elements[3][2] -
                        mat.Elements[1][0] * mat.Elements[0][2] * mat.Elements[3][1] -
                        mat.Elements[3][0] * mat.Elements[0][1] * mat.Elements[1][2] +
                        mat.Elements[3][0] * mat.Elements[0][2] * mat.Elements[1][1];

    result.Elements[0][3] = -mat.Elements[0][1] * mat.Elements[1][2] * mat.Elements[2][3] +
                        mat.Elements[0][1] * mat.Elements[1][3] * mat.Elements[2][2] +
                        mat.Elements[1][1] * mat.Elements[0][2] * mat.Elements[2][3] -
                        mat.Elements[1][1] * mat.Elements[0][3] * mat.Elements[2][2] -
                        mat.Elements[2][1] * mat.Elements[0][2] * mat.Elements[1][3] +
                        mat.Elements[2][1] * mat.Elements[0][3] * mat.Elements[1][2];

    result.Elements[1][3] = mat.Elements[0][0] * mat.Elements[1][2] * mat.Elements[2][3] -
                        mat.Elements[0][0] * mat.Elements[1][3] * mat.Elements[2][2] -
                        mat.Elements[1][0] * mat.Elements[0][2] * mat.Elements[2][3] +
                        mat.Elements[1][0] * mat.Elements[0][3] * mat.Elements[2][2] +
                        mat.Elements[2][0] * mat.Elements[0][2] * mat.Elements[1][3] -
                        mat.Elements[2][0] * mat.Elements[0][3] * mat.Elements[1][2];

    result.Elements[2][3] = -mat.Elements[0][0] * mat.Elements[1][1] * mat.Elements[2][3] +
                        mat.Elements[0][0] * mat.Elements[1][3] * mat.Elements[2][1] +
                        mat.Elements[1][0] * mat.Elements[0][1] * mat.Elements[2][3] -
                        mat.Elements[1][0] * mat.Elements[0][3] * mat.Elements[2][1] -
                        mat.Elements[2][0] * mat.Elements[0][1] * mat.Elements[1][3] +
                        mat.Elements[2][0] * mat.Elements[0][3] * mat.Elements[1][1];

    result.Elements[3][3] = mat.Elements[0][0] * mat.Elements[1][1] * mat.Elements[2][2] -
                        mat.Elements[0][0] * mat.Elements[1][2] * mat.Elements[2][1] -
                        mat.Elements[1][0] * mat.Elements[0][1] * mat.Elements[2][2] +
                        mat.Elements[1][0] * mat.Elements[0][2] * mat.Elements[2][1] +
                        mat.Elements[2][0] * mat.Elements[0][1] * mat.Elements[1][2] -
                        mat.Elements[2][0] * mat.Elements[0][2] * mat.Elements[1][1];

    double det = mat.Elements[0][0] * result.Elements[0][0] +
                    mat.Elements[0][1] * result.Elements[1][0] +
                    mat.Elements[0][2] * result.Elements[2][0] +
                    mat.Elements[0][3] * result.Elements[3][0];
    if(det == 0){//may be best to refactor this to pass in a pointer instead
        Mat4 fail;
        for(int x=0;x<4;x++){
            for(int y=0;y<4;y++){
                fail.Elements[x][y] = 0;
            }
        }
        return fail;
    }
    det = 1.0 / det;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            result.Elements[i][j] = result.Elements[i][j] * (float)det;
        }
    }
    return result; 
}

Mat4 perspectiveMat4(float fov,float aspect,float close,float distant){
    float invTan = 1.0f/tanf(fov/2);

    Mat4 result;
    result.Elements[0][0] = invTan/aspect;
    result.Elements[1][0] = 0.0f;
    result.Elements[2][0] = 0.0f; 
    result.Elements[3][0] = 0.0f;

    result.Elements[0][1] = 0.0f;
    result.Elements[1][1] = invTan;
    result.Elements[2][1] = 0.0f;
    result.Elements[3][1] = 0.0f;

    result.Elements[0][2] = 0.0f;
    result.Elements[1][2] = 0.0f;
    result.Elements[2][2] = -((close+distant) / (distant-close));
    result.Elements[3][2] =  -((2*distant*close)/(distant-close));

    result.Elements[0][3] = 0.0f;
    result.Elements[1][3] = 0.0f;
    result.Elements[2][3] = -1.0f;
    result.Elements[3][3] = 0.0f;

    return result;
}
Mat4 perspectiveMat4_Z0(float fov,float aspect,float close,float distant){
    float invTan = 1.0f/tanf(fov/2);

    Mat4 result;
    result.Elements[0][0] = invTan/aspect;
    result.Elements[1][0] = 0.0f;
    result.Elements[2][0] = 0.0f; 
    result.Elements[3][0] = 0.0f;

    result.Elements[0][1] = 0.0f;
    result.Elements[1][1] = invTan;
    result.Elements[2][1] = 0.0f;
    result.Elements[3][1] = 0.0f;

    result.Elements[0][2] = 0.0f;
    result.Elements[1][2] = 0.0f;
    result.Elements[2][2] = distant / (close-distant);
    result.Elements[3][2] =  ((close*distant)/(close-distant));

    result.Elements[0][3] = 0.0f;
    result.Elements[1][3] = 0.0f;
    result.Elements[2][3] = -1.0f;
    result.Elements[3][3] = 0.0f;

    return result;
}
void scaleMat4(Mat4 *src,Vector3 scale){//Modify a mat4 with a scale vector
    src->Elements[0][0] *= scale.x;
    src->Elements[1][1] *= scale.y;
    src->Elements[2][2] *= scale.z;  
}
Mat4 scaledMat4(Mat4 src, Vector3 scale){//if you want to keep the original mat4 as well this will return a mat4
    Mat4 result =  src;
    
    result.Elements[0][0] *= scale.x;
    result.Elements[1][1] *= scale.y;
    result.Elements[2][2] *= scale.z; 
    
    return result;
}
Mat4 zeroMat4(){
    Mat4 result;
    for(int x=0;x<4;x++){
        for(int y=0;y<4;y++){
            result.Elements[x][y] = 0;
        }
    }
    return result;
}
Mat4 TRSMat4(Vector3 translate,Vector4 rotate,Vector3 scale){
    Mat4 result;

    //Vector4 normQuaternion = normalizeVec4(rotate);

    float XX = rotate.x * rotate.x;
    float YY = rotate.y * rotate.y;
    float ZZ = rotate.z * rotate.z;
    float XY = rotate.x * rotate.y;
    float XZ = rotate.x * rotate.z;
    float YZ = rotate.y * rotate.z;
    float WX = rotate.w * rotate.x;
    float WY = rotate.w * rotate.y;
    float WZ = rotate.w * rotate.z;


    
    result.Elements[0][0] = 1-2*(YY+ZZ) *scale.x;
    result.Elements[0][1] = 2*(XY + WZ)*scale.x;
    result.Elements[0][2] = 2*(XZ - WY)*scale.x;
    result.Elements[0][3] = 0;

    result.Elements[1][0] = 2*(XY-WZ)*scale.y;
    result.Elements[1][1] = 1-2*(XX+ZZ)*scale.y;
    result.Elements[1][2] = 2*(YZ + WX)*scale.y;
    result.Elements[1][3] = 0;

    result.Elements[2][0] = 2*(XZ + WY)*scale.z;
    result.Elements[2][1] = 2*(YZ - WX)*scale.z;
    result.Elements[2][2] = 1-2*(XX + YY)*scale.z;
    result.Elements[2][3] = 0;

    result.Elements[3][0] = translate.x;
    result.Elements[3][1] = translate.y;
    result.Elements[3][2] = translate.z;
    result.Elements[3][3] = 1;

    return result;
}
void decomposeTRSMat4(Mat4 TRS,Vector3 *translate,Vector4 *rotation,Vector3 *scale){//UNFINISHED

    rotation;
    //*translate = (Vector3){TRS.Elements[3][0],TRS.Elements[3][1],TRS.Elements[3][2]};
    translate->x = TRS.Elements[3][0];
    translate->y = TRS.Elements[3][1];
    translate->z = TRS.Elements[3][2];

    Vector3 temp;
    temp.x = TRS.Elements[0][0];
    temp.y = TRS.Elements[0][1];
    temp.z = TRS.Elements[0][2];

    scale->x = magVec3(temp);

    temp.x = TRS.Elements[1][0];
    temp.y = TRS.Elements[1][1];
    temp.z = TRS.Elements[1][2];

    scale->y = magVec3(temp);

    temp.x = TRS.Elements[2][0];
    temp.y = TRS.Elements[2][1];
    temp.z = TRS.Elements[2][2];

    scale->z = magVec3(temp);
}

//-----------------------------------------------------------------------------------------------------------
// QUATERNIONS
//-----------------------------------------------------------------------------------------------------------

Mat4 quaternionToRotationMatrix(Vector4 quaternion){//Nice implementation from handmademath. Same as wikipedia article
    //first we need to normalize the quaternion
    Vector4 normQuaternion = normalizeVec4(quaternion);

    float XX = normQuaternion.x * normQuaternion.x;
    float YY = normQuaternion.y * normQuaternion.y;
    float ZZ = normQuaternion.z * normQuaternion.z;
    float XY = normQuaternion.x * normQuaternion.y;
    float XZ = normQuaternion.x * normQuaternion.z;
    float YZ = normQuaternion.y * normQuaternion.z;
    float WX = normQuaternion.w * normQuaternion.x;
    float WY = normQuaternion.w * normQuaternion.y;
    float WZ = normQuaternion.w * normQuaternion.z;

    Mat4 result;
    result.Elements[0][0] = 1-2*(YY+ZZ);
    result.Elements[1][0] = 2*(XY-WZ);
    result.Elements[2][0] = 2*(XZ + WY);
    result.Elements[3][0] = 0;

    result.Elements[0][1] = 2*(XY + WZ);
    result.Elements[1][1] = 1-2*(XX+ZZ);
    result.Elements[2][1] = 2*(YZ - WX);
    result.Elements[3][1] = 0;

    result.Elements[0][2] = 2*(XZ - WY);
    result.Elements[1][2] = 2*(YZ + WX);
    result.Elements[2][2] = 1-2*(XX + YY);
    result.Elements[3][2] = 0;

    result.Elements[0][3] = 0;
    result.Elements[1][3] = 0;
    result.Elements[2][3] = 0;
    result.Elements[3][3] = 1;

    return result;
    
}

//https://github.com/KhronosGroup/glTF-Tutorials/blob/main/gltfTutorial/gltfTutorial_007_Animations.md
Vector4 slerpQuaternion(Vector4 A, Vector4 B, float t){
    Vector4 result;
    float dotProduct = dotVec4(A,B);

    if(dotProduct<0.0){
        B=mulVec4Float(B,-1);
        dotProduct = -1*dotProduct;
    }
    
    if(dotProduct>0.9995){
        return normalizeVec4(lerpVec4(A,B,t));
    }
    
    float theta_0 = acosf(dotProduct);
    float theta = t*theta_0;
    float sin_theta = sinf(theta);
    float sin_theta_0 = sinf(theta_0);

    float scalePreviousQuaternion = cosf(theta) - dotProduct * sin_theta / sin_theta_0;
    float scaleNextQuaternion =  sin_theta / sin_theta_0;
    Vector4 scaledAVec4 = mulVec4Float(A,scalePreviousQuaternion);
    Vector4 scaledBVec4 = mulVec4Float(B,scaleNextQuaternion);
    result = addVec4(scaledAVec4,scaledBVec4);

    return result;
}


#endif