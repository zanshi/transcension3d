#pragma once
class Texture3D{
public:
  Texture3D(GLuint w, GLuint h, GLuint d){
    this->w = w; 
    this->h = h;
    this->d = d;
    this->nChannels = 4;

    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_3D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);	
    data = new GLfloat[h*w*d*nChannels];
    for (unsigned int i = 0; i < w; ++i) {
      for (unsigned int j = 0; j < h; ++j){
        for (unsigned int k = 0; k < d; ++k){
          data[indexTranslation(i,j,k,0)] = 0.0f;
          data[indexTranslation(i,j,k,1)] = 0.0f;
          data[indexTranslation(i,j,k,2)] = 0.0f;
          data[indexTranslation(i,j,k,3)] = 1.0f;
        }
      }
    }
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, w, h, d, 0, GL_RGBA, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // enforce no texture wrap
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  }
  ~Texture3D(){
    glDeleteTextures(1, &textureID);
    delete[] data;
  }
  GLfloat get(GLuint i, GLuint j, GLuint k, GLuint, GLuint c){
    return data[indexTranslation(i, j, k, c)];
  }
  void set(GLuint i, GLuint j, GLuint k, GLuint c, GLfloat v){
    data[indexTranslation(i, j, k, c)] = v;
  }
  operator GLuint(){
    return textureID;
  }
  void operator()(GLenum texture) { 
    glActiveTexture(texture);
    glBindTexture(GL_TEXTURE_3D, textureID);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, w, h, d, 0, GL_RGBA, GL_FLOAT, data);
  }
  
private:
  GLuint w, h, d, nChannels;
  GLuint textureID; 
  GLfloat *data;
  GLuint indexTranslation(GLuint i, GLuint j, GLuint k, GLuint c){
    return nChannels*(w*(k*h + j) + i) + c;
  }
};
