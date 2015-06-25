#include <iostream>
#include <fstream>
#include <vector>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << "<N> [images]" << std::endl;
        return 1;
    }

    int imgs = atoi(argv[1]);

    if (imgs == 0) {
        std::cerr << "No images to work on." << std::endl;
        return 0;
    }

    if (imgs > 10) {
        std::cerr << "Number of images limit exceeded." << std::endl;
        return 0;
    }

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init(NULL, "ell")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }

    // Open input images with leptonica library and store them in a vector
    std::vector<Pix *> letter_imgs;

    int args_offset = 2;
    for (int i = 0; i < imgs; ++i)
        letter_imgs.push_back( pixRead( argv[ args_offset + i ] ) );

    std::ofstream output;
    output.open("output.txt");
    std::vector<Pix *>::iterator it;
    for (it = letter_imgs.begin(); it != letter_imgs.end(); ++it)
    {
        api->SetImage(*it);
        // Get OCR result
        output << api->GetUTF8Text();
    }
    output.close();

    // Destroy used object and release memory
    api->End();
    for (it = letter_imgs.begin(); it != letter_imgs.end(); ++it)
        pixDestroy(&(*it));

    return 0;
}
