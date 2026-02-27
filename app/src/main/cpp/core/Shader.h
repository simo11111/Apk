#ifndef SHADER_H
#define SHADER_H

#include <GLES3/gl3.h>
#include <string.h>

class Shader {
public:
    GLuint program;
    GLint mvpHandle;
    GLint posHandle;
    GLint colorHandle;
    GLint alphaHandle;

    Shader() : program(0) {}

    GLuint loadShader(GLenum type, const char* src) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, NULL);
        glCompileShader(shader);
        return shader;
    }

    void init() {

        const char* vShader =
            "#version 300 es\n"
            "layout(location = 0) in vec3 aPos;\n"
            "uniform mat4 uMVP;\n"
            "void main() {\n"
            "   gl_Position = uMVP * vec4(aPos,1.0);\n"
            "}";

        const char* fShader =
            "#version 300 es\n"
            "precision mediump float;\n"
            "uniform vec4 uColor;\n"
            "uniform float uAlpha;\n"
            "out vec4 FragColor;\n"
            "void main() {\n"
            "   FragColor = vec4(uColor.rgb, uAlpha);\n"
            "}";

        GLuint vs = loadShader(GL_VERTEX_SHADER, vShader);
        GLuint fs = loadShader(GL_FRAGMENT_SHADER, fShader);

        program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        glDeleteShader(vs);
        glDeleteShader(fs);

        mvpHandle = glGetUniformLocation(program, "uMVP");
        colorHandle = glGetUniformLocation(program, "uColor");
        alphaHandle = glGetUniformLocation(program, "uAlpha");
        posHandle = 0; // location 0 (layout)
    }

    void use() {
        glUseProgram(program);
    }

    void setAlpha(float a) {
        glUniform1f(alphaHandle, a);
    }
};

#endif
