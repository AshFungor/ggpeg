#pragma once
#include <image/image.hpp>

namespace proc {
    // Code for processing functions.
    /** \brief Imposes a negative filter on the image.
     * \param img The image to which the filter is applied.
     */
    void negative(img::Image &img);
    /** \brief Crops the image.
     * \param img The image that is being cropped.
     * \param left The number of percentages by which the image will be cropped from \a left.
     * \param top The number of percentages by which the image will be cropped from \a top.
     * \param right The number of percentages by which the image will be cropped from \a right.
     * \param bottom The number of percentages by which the image will be cropped from \a bottom.
     * \details This function does not perform any range-checking for \a left, \a top, \a right, \a bottom. 
     * The function cuts the picture on each side depending on the parameters \a left, \a top, \a right, \a bottom.
     * For instance, left means that \a left pixels
     * are cut from the left.
     */
    void crop(img::Image &img, double left, double top, double right, double bottom);
    /** \brief Inserts one image into another.
     * \param img The image on which the \a other image will be inserted.
     * \param other The image that will be inserted on \a img image.
     * \param x The x-coordinate on which the left-top upper pixel of the \a other image will be inserted.
     * \param y The y-coordinate on which the left-top upper pixel of the \a other image will be inserted.
     * \details The function insert \a other image on the \a img image starting from the pixel with the \a x and \a y coordinates.
     * Note, if the \a x and \a y parameters are selected in such a way that the \a other image does not fall on the \a img image, 
     * then there will be no changes in the \a img image.
     */
    void insert(img::Image &img, img::Image &other, int x, int y);
    /** \brief Reflects the image relative to the x axis.
     * \param img The image that will be reflected relative to the x axis.
     */
    void reflex_x(img::Image &img);
    /** \brief Reflects the image relative to the y axis.
     * \param img The image that will be reflected relative to the y axis.
     */
    void reflex_y(img::Image &img);
    /** \brief Сhanges the image resolution.
     * \param img The image whose resolution is being changed.
     * \param k Multiplicity of resolution changes.
     * \details The funtion changes the resolution of the \a img image by \a k times.
     * Note, if the value of \a k is less than or equal to zero, the \a img image will not be changed.
     */
    void resize(img::Image &img, double k);
    /** \brief Rotates the image.
     * \param img The image to be rotated.
     * \param degrees The number of degrees by which the \a img image will be rotated.
     * \details Function rotates the \a img image by \a degrees degrees from the center of the \a img image.
     * If the value of \a degrees parameter is positive, then the rotation is counterclockwise. 
     * If the value of the \a degrees parameter is negative, then the rotation is clockwise.
     */
    void rotate(img::Image& img, double degrees);
}
