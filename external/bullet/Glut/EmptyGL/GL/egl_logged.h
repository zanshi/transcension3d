inline void glFinish() {xGLL(EGL_TOKEN_glFinish);};
inline void glClear(GLbitfield  a0) {xGLL(EGL_TOKEN_glClear);xGLL(a0);};
inline void glFlush() {xGLL(EGL_TOKEN_glFlush);};
inline void glMatrixMode(GLenum  a0) {xGLL(EGL_TOKEN_glMatrixMode);xGLL(a0);};
inline void glOrtho(GLdouble  a0,GLdouble  a1,GLdouble  a2,GLdouble  a3,GLdouble  a4,GLdouble  a5) {xGLL(EGL_TOKEN_glOrtho);xGLL(a0);xGLL(a1);xGLL(a2);xGLL(a3);xGLL(a4);xGLL(a5);};
inline void glFrustum(GLdouble  a0,GLdouble  a1,GLdouble  a2,GLdouble  a3,GLdouble  a4,GLdouble  a5) {xGLL(EGL_TOKEN_glFrustum);xGLL(a0);xGLL(a1);xGLL(a2);xGLL(a3);xGLL(a4);xGLL(a5);};
inline void glViewport(GLint  a0,GLint  a1,GLsizei  a2,GLsizei  a3) {xGLL(EGL_TOKEN_glViewport);xGLL(a0);xGLL(a1);xGLL(a2);xGLL(a3);};
inline void glPushMatrix() {xGLL(EGL_TOKEN_glPushMatrix);};
inline void glPopMatrix() {xGLL(EGL_TOKEN_glPopMatrix);};
inline void glLoadIdentity() {xGLL(EGL_TOKEN_glLoadIdentity);};
inline void glLoadMatrixd(const GLdouble *  a0) {xGLL(EGL_TOKEN_glLoadMatrixd);xGLL(a0);};
inline void glLoadMatrixf(const GLfloat *  a0) {xGLL(EGL_TOKEN_glLoadMatrixf);xGLL(a0);};
inline void glMultMatrixd(const GLdouble *  a0) {xGLL(EGL_TOKEN_glMultMatrixd);xGLL(a0);};
inline void glMultMatrixf(const GLfloat *  a0) {xGLL(EGL_TOKEN_glMultMatrixf);xGLL(a0);};
inline void glRotated(GLdouble  a0,GLdouble  a1,GLdouble  a2,GLdouble  a3) {xGLL(EGL_TOKEN_glRotated);xGLL(a0);xGLL(a1);xGLL(a2);xGLL(a3);};
inline void glRotatef(GLfloat  a0,GLfloat  a1,GLfloat  a2,GLfloat  a3) {xGLL(EGL_TOKEN_glRotatef);xGLL(a0);xGLL(a1);xGLL(a2);xGLL(a3);};
inline void glScaled(GLdouble  a0,GLdouble  a1,GLdouble  a2) {xGLL(EGL_TOKEN_glScaled);xGLL(a0);xGLL(a1);xGLL(a2);};
inline void glScalef(GLfloat  a0,GLfloat  a1,GLfloat  a2) {xGLL(EGL_TOKEN_glScalef);xGLL(a0);xGLL(a1);xGLL(a2);};
inline void glTranslated(GLdouble  a0,GLdouble  a1,GLdouble  a2) {xGLL(EGL_TOKEN_glTranslated);xGLL(a0);xGLL(a1);xGLL(a2);};
inline void glTranslatef(GLfloat  a0,GLfloat  a1,GLfloat  a2) {xGLL(EGL_TOKEN_glTranslatef);xGLL(a0);xGLL(a1);xGLL(a2);};
