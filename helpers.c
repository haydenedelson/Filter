#include "helpers.h"
#include <math.h>
#include <stdio.h>

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    int avg;

    // Compute average of R, G, and B values for each pixel
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            avg = round(((float)(image[i][j].rgbtBlue + image[i][j].rgbtGreen + image[i][j].rgbtRed)) / 3);

            // Assign average value to pixel
            image[i][j].rgbtBlue = avg;
            image[i][j].rgbtGreen = avg;
            image[i][j].rgbtRed = avg;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    int mid = width / 2;
    RGBTRIPLE temp;

    // Swap pixels equidistant from edges of image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < mid; j++)
        {
            temp = image[i][j];
            image[i][j] = image[i][width - j - 1];
            image[i][width - j - 1] = temp;
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    int i, j, k, l, count;
    int total_red, total_blue, total_green;
    int avg_red, avg_blue, avg_green;

    // Temporary placeholder image
    RGBTRIPLE copy[height][width];

    // For each pixel, compute total amount of R, G, and B in surrounding pixels
    // As an intermediate step, assign blurred pixel to temporary placeholder image, so that original is not altered
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            // Reset totals and counter to 0 for current pixel
            total_red = 0;
            total_blue = 0;
            total_green = 0;
            count = 0;

            // For each pixel, sum total colors in pixels from image[i-1][j-1] to image[i+1][j+1] (or whatever range exists)
            for (k = i - 1; k <= i + 1; k++)
            {
                for (l = j - 1; l <= j + 1; l++)
                {
                    // If current pixel does not exist, ignore
                    if (!(k < 0 || l < 0 || k > height - 1 || l > width - 1))
                    {
                        total_red += image[k][l].rgbtRed;
                        total_blue += image[k][l].rgbtBlue;
                        total_green += image[k][l].rgbtGreen;
                        count++;
                    }
                }
            }
            // Compute averages
            avg_red = round((float) total_red / count);
            avg_blue = round((float) total_blue / count);
            avg_green = round((float) total_green / count);
            
            // Assign averages to temporary placeholder so that original image is not altered
            copy[i][j].rgbtRed = avg_red;
            copy[i][j].rgbtBlue = avg_blue;
            copy[i][j].rgbtGreen = avg_green;
        }
    }

    // Copy placeholder image to original image for output
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            image[i][j] = copy[i][j];
        }
    }
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // Helper array for storing RGB values of surrounding pixels
    RGBTRIPLE edges[3][3];
    // Temporary placeholder image
    RGBTRIPLE copy[height][width];

    // Gx matrix
    int Gx[3][3] =
    {
        { -1, 0, 1 },
        { -2, 0, 2 },
        { -1, 0, 1 }
    };

    // Gy matrix
    int Gy[3][3] =
    {
        { -1, -2, -1 },
        { 0, 0, 0 },
        { 1, 2, 1 }
    };

    int i, j, k, l, row, col;


    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            // Populate 3x3 edges matrix with RGB values of pixels [i-1][j-1] to [i+1][j+1]
            // If surrounding pixels do not exist, set RGB values to 0
            for (k = i - 1, row = 0; k <= i + 1; k++, row++)
            {
                for (l = j - 1, col = 0; l <= j + 1; l++, col++)
                {
                    // If pixel does not exist, set RGB values to 0
                    if (k < 0 || k > height - 1 || l < 0 || l > width - 1)
                    {
                        edges[row][col].rgbtRed = 0;
                        edges[row][col].rgbtGreen = 0;
                        edges[row][col].rgbtBlue = 0;
                    }
                    else
                    {
                        edges[row][col].rgbtRed = image[k][l].rgbtRed;
                        edges[row][col].rgbtGreen = image[k][l].rgbtGreen;
                        edges[row][col].rgbtBlue = image[k][l].rgbtBlue;
                    }
                }
            }

            // Calculate Gx and Gy values
            // For each cell in Gx and Gy matrices, multiply by corresponding cell in edges matrix
            int Gx_red = 0, Gx_green = 0, Gx_blue = 0;
            int Gy_red = 0, Gy_green = 0, Gy_blue = 0;

            for (k = 0; k < 3; k++)
            {
                for (l = 0; l < 3; l++)
                {
                    // Calculate pixel red
                    Gx_red += edges[k][l].rgbtRed * Gx[k][l];
                    Gy_red += edges[k][l].rgbtRed * Gy[k][l];

                    // Calculate pixel green
                    Gx_green += edges[k][l].rgbtGreen * Gx[k][l];
                    Gy_green += edges[k][l].rgbtGreen * Gy[k][l];

                    // Calculate pixel blue
                    Gx_blue += edges[k][l].rgbtBlue * Gx[k][l];
                    Gy_blue += edges[k][l].rgbtBlue * Gy[k][l];
                }
            }

            // Calculate final RGB values according to Sobel filter algorithm
            // Assign values to temporary placeholder image so that original image is not altered

            // Assign red
            WORD red = round(sqrt((Gx_red * Gx_red) + (Gy_red * Gy_red)));
            copy[i][j].rgbtRed = (red > 255) ? 255 : red;

            // Assign green
            WORD green = round(sqrt((Gx_green * Gx_green) + (Gy_green * Gy_green)));
            copy[i][j].rgbtGreen = (green > 255) ? 255 : green;

            // Assign blue
            WORD blue = round(sqrt((Gx_blue * Gx_blue) + (Gy_blue * Gy_blue)));
            copy[i][j].rgbtBlue = (blue > 255) ? 255 : blue;
        }
    }

    // Copy placeholder image to original image for output
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            image[i][j] = copy[i][j];
        }
    }
    return;
}
