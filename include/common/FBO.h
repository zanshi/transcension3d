#include <iostream>
#include <common/Texture2D.h>

class FBO {
public:
  FBO(GLuint w, GLuint h) {
    this->w = w;
    this->h = h;

    // create new framebuffer
    glGenFramebuffers(1, &framebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);

    // create texture
    texture = new Texture2D(w, h);

    // init config
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texture, 0);

    // fail check
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "failed to init FBO" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  };

  ~FBO() {
    glDeleteFramebuffers(1, &framebufferId);
    delete texture;
  };

  
  void activate() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
  };


  static void deactivate() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  };

  
  operator const GLuint() const {
    return framebufferId;
  }

  static void deactivateFramebuffers() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  };


  Texture2D const *const getTexture() const {
    return texture;
  };

  void download() {
    texture->download();
  }

private:
  GLuint w, h;
  GLuint framebufferId;
  Texture2D *texture;
  GLenum texUnit;
};
