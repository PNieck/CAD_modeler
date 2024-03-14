#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>


class Object {
public:
    Object();

    glm::mat4x4 model_matrix() const;

    void rotate_x(float angle) { rotation.x += angle; }
    void rotate_y(float angle) { rotation.y += angle; }
    void rotate_z(float angle) { rotation.z += angle; }

    void SetScale(float scale) { this->scale = scale; }

    void MoveX(float x) { position.x += x; }
    void MoveY(float y) { position.y += y; }
    void MoveZ(float z) { position.z += z; }

protected:
    glm::vec3 position;
    glm::vec3 rotation;
    float scale;

private:
    glm::mat4x4 scale_matrix() const;
    glm::mat4x4 rotation_matrix() const;
    glm::mat4x4 translation_matrix() const;

    glm::mat4x4 rotation_x_matrix() const;
    glm::mat4x4 rotation_y_matrix() const;
    glm::mat4x4 rotation_z_matrix() const;
};
