#ifndef CAMERA_H
#define CAMERA_H

#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace cam
{
// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


class Camera
{
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
        float yaw = YAW, float pitch = PITCH, float max_fov = 45);

    Camera(float posX, float posY, float posZ, 
        float upX, float upY, float upZ, 
        float yaw, float pitch, float max_fov = 45);
    ~Camera();

    // returns the current camera view matrix
    const glm::mat4 get_view_matrix() const;
    const float get_fov() const;
    Camera set_max_fov(float max_fov);
    // moves the camera based off of the input received
    void move(Movement direction, float dt);
    // rotates the camera angles given the inputs
    void rotate(float x_offset, float y_offset, bool constrain_pitch = true);
    // zooms the camera in or out given the offset
    void zoom(float y_offset);


private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void update_vectors();
    
    // camera attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;
    // angles
    float yaw;
    float pitch;
    // options
    float speed;
    float sensitivity;
    float fov;
    float max_fov;
};

}; // namespace camera


cam::Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float max_fov) : 
        front(glm::vec3(0.0f, 0.0f, -1.0f)), speed(cam::SPEED), 
        sensitivity(cam::SENSITIVITY), fov(cam::ZOOM), position(position),
        world_up(up), yaw(yaw), pitch(pitch), max_fov(max_fov)
{
    this->update_vectors();
}

cam::Camera::Camera(float posX, float posY, float posZ, 
        float upX, float upY, float upZ, float yaw, float pitch, float max_fov) : 
        front(glm::vec3(0.0f, 0.0f, -1.0f)), speed(cam::SPEED), 
        sensitivity(cam::SENSITIVITY), fov(cam::ZOOM), position(glm::vec3(posX, posY, posZ)),
        world_up(glm::vec3(upX, upY, upZ)), yaw(yaw), pitch(pitch), max_fov(max_fov)
{
    this->update_vectors();
}

cam::Camera::~Camera() { }


void cam::Camera::move(cam::Movement direction, float dt)
{
    float velocity = speed * dt;
    if (direction == cam::FORWARD)
        this->position += this->front * velocity;
    if (direction == cam::BACKWARD)
        this->position -= this->front * velocity;
    if (direction == cam::LEFT)
        this->position -= this->right * velocity;
    if (direction == cam::RIGHT)
        this->position += this->right * velocity;
}


void cam::Camera::rotate(float x_offset, float y_offset, bool constrain_pitch)
{
    x_offset *= sensitivity;
    y_offset *= sensitivity;

    this->yaw   += x_offset;
    this->pitch += y_offset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrain_pitch)
    {
        if (this->pitch > 89.0f)
            this->pitch = 89.0f;
        if (this->pitch < -89.0f)
            this->pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    this->update_vectors();
}


void cam::Camera::zoom(float y_offset)
{
    this->fov -= y_offset;
    if (this->fov < 1.0f)
        this->fov = 1.0f;
    if (this->fov > this->max_fov)
        this->fov = this->max_fov; 
}


void cam::Camera::update_vectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    front.y = sin(glm::radians(this->pitch));
    front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    this->front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    // normalize the vectors, because their length gets closer to 0 
    // the more you look up or down which results in slower movement.
    this->right = glm::normalize(glm::cross(this->front, this->world_up));
    this->up = glm::normalize(glm::cross(this->right, this->front));
}


const glm::mat4 cam::Camera::get_view_matrix() const
{
    return glm::lookAt(this->position, this->position + this->front, this->up);
}


const float cam::Camera::get_fov() const
{
    return this->fov;
}


cam::Camera cam::Camera::set_max_fov(float max_fov)
{
    this->max_fov = max_fov;
    return *this;
}

#endif