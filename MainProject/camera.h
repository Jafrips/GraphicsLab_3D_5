#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

struct SCamera {
    float x,y,z;
    float Xrot,Zrot;
};

void Camera_Apply();
void Camera_Rotation(float xAngle, float zAngle);
void Camera_MouseMovement(int centerX, int centerY, float speed);
void Camera_DirectionalMovement(int forwardMove, int rightMove, float speed);

#endif // CAMERA_H_INCLUDED
