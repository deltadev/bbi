#include <getopt.h>

#include <iostream>

struct opts {

    std::string resource;
    std::string contig;

    std::string host = "localhost";
    std::string port = "http";

    bool print_chrom_ids = false;
    bool print_main_headers = false;
    bool print_zoom_headers = false;

    uint32_t m = 0;
    uint32_t M = 0;
    uint32_t zoom_level = 0;

    bool debug_session = false;
};

void usage() {
    std::cerr <<

        "Usage:\n"
        "\t-r, --resource       required (http:// or local file)\n"

        "\t-c, --chrom-name     (defaults fo first index)\n"
        "\t-m, --min-coord      (defaults to 0)\n"
        "\t-M, --max-coord      (defaults to 0)\n\n"

        "\t-h, --host           (defaults to localhost)\n"

        "\t-p, --port           (defaults to 80)\n"
        "\t-z, --zoom           (defaults to 0 i.e. main data)\n"
        "\t-d, --debug-session  prints the headers for the HTTP session\n\n"

        "\t--print-chroms       prints each chromosome, (name, id, size)\n"
        "\t--print-main         prints the main-headers\n"
        "\t--print-zoom         prints the zoom-headers\n";
}

opts parse_options(int argc, char** argv) {
    opts opts;
    int print = 0;
    option longopts[]{
        {"host", required_argument, NULL, 'h'},
        {"port", required_argument, NULL, 'p'},
        {"resource", required_argument, NULL, 'r'},
        {"zoom-level", required_argument, NULL, 'z'},
        {"chrom-name", required_argument, NULL, 'c'},
        {"print-chroms", no_argument, &print, 1},
        {"print-main", no_argument, &print, 2},
        {"print-zoom", no_argument, &print, 3},
        {"min-coord", required_argument, NULL, 'm'},
        {"max-coord", required_argument, NULL, 'M'},
        {"debug-session", no_argument, NULL, 'd'},
        {NULL, 0, NULL, 0} // This is a required sentinel.
    };

    char c;
    while ((c = getopt_long_only(argc, argv, "h:p:r:z:m:M:d", longopts, NULL)) != -1) {
        switch (c) {
        case 'h':
            opts.host.assign(optarg);
            break;
        case 'p':
            opts.port.assign(optarg);
            break;
        case 'r':
            opts.resource.assign(optarg);
            break;
        case 'c':
            opts.contig.assign(optarg);
            break;
        case 'z':
            opts.zoom_level = std::atoi(optarg);
            break;
        case 'm':
            opts.m = std::atoi(optarg);
            break;
        case 'M':
            opts.M = std::atoi(optarg);
            break;
        case 'd':
            opts.debug_session = true;
            break;
        case 0:
            switch (print) {
            case 1:
                opts.print_chrom_ids = true;
                break;
            case 2:
                opts.print_main_headers = true;
                break;
            case 3:
                opts.print_zoom_headers = true;
                break;
            default:
                break;
            }
            break;
        case ':':
            usage();
        case '?':
            usage();
        default:
            exit(EXIT_FAILURE);
            break;
        }
    }

    if (opts.resource.empty()) {
        std::cerr << "resource is a required option.\n";
        usage();
        exit(EXIT_FAILURE);
    }

    return opts;
}
