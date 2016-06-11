#include <iostream>
#include <fstream>
#include <vector>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#define ONEPIECE

int main(int argc, char *argv[])
{
    if (argc == 1) {
        std::cerr << "No images to work on." << std::endl;
        return 1;
    }

    // Initialize tesseract-ocr with English, without specifying tessdata path
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    if (api->Init(NULL, "eng")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }

    std::ofstream output;
    output.open("/tmp/glpr-output/output.txt");


#ifdef ONEPIECE
    Pix *plate;

    plate = pixRead(argv[1]);
    api->SetImage(plate);
    output << api->GetUTF8Text();
    std::cout << "LPR:\n" << api->GetUTF8Text();

    output.close();

    api->End();
    pixDestroy(&plate);

#else
    // Open input images with leptonica library and store them in a vector
    std::vector<Pix *> letter_imgs;

    int args_offset = 1;
    for (int i = 0; i < argc-1; ++i)
        letter_imgs.push_back( pixRead( argv[ args_offset + i ] ) );

    std::vector<Pix *>::iterator it;

//#pragma omp parallel for
    for (it = letter_imgs.begin(); it < letter_imgs.end(); ++it)
    {
        //std::cout << "*" << std::endl;
        api->SetImage(*it);
        // Get OCR result
        output << api->GetUTF8Text();
    }
    output.close();

    // Destroy used object and release memory
    api->End();
    for (it = letter_imgs.begin(); it != letter_imgs.end(); ++it)
        pixDestroy(&(*it));
#endif

    return 0;
}
