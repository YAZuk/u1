#ifndef U2_UTILS_H
#define U2_UTILS_H

void drawAirplane(float centerX, float centerY) {
    glBegin(GL_LINES);
    glVertex2f(centerX, centerY + 20);
    glVertex2f(centerX, centerY - 20);
    glVertex2f(centerX - 20, centerY);
    glVertex2f(centerX + 20, centerY);
    glEnd();
}


void drawCircle(float x0, float y0, float radius, int countSegments) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < countSegments; i++) {
        float theta = 2.0f * M_PI * float(i) / float(countSegments);
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        glVertex2f(x + x0, y + y0);
    }
    glEnd();
}

//void renderBitmapString(
//        float x,
//        float y,
//        float z,
//        void *font,
//        char *string) {
//
//    char *c;
//    glRasterPos3f(x, y, z);
//    for (c = string; *c != '\0'; c++)
//        glutBitmapCharacter(font, *c);
//}

void renderBitmapString(
        float x,
        float y,
        float z,
        void *font,
        const char *string) {

    glRasterPos3f(x, y, z);
    for (; *string != '\0'; string++)
        glutBitmapCharacter(font, *string);
}


//void renderSpacedBitmapString(
//        float x,
//        float y,
//        int spacing,
//        void *font,
//        char *string) {
//
//    char *c;
//    int x1 = x;
//
//    for (c = string; *c != '\0'; c++) {
//
//        glRasterPos2f(x1, y);
//        glutBitmapCharacter(font, *c);
//        x1 = x1 + glutBitmapWidth(font, *c) + spacing;
//    }
//}
//
//void renderVerticalBitmapString(
//
//        float x,
//        float y,
//        int bitmapHeight,
//        void *font,
//        char *string) {
//
//    char *c;
//    int i;
//
//    for (c = string, i = 0; *c != '\0'; i++, c++) {
//
//        glRasterPos2f(x, y + bitmapHeight * i);
//        glutBitmapCharacter(font, *c);
//    }
//}


#endif //U2_UTILS_H
