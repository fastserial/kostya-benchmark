#include <libnotify.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <lite3_context_api.h>


#ifdef __clang__
#define COMPILER "clang"
#else
#define COMPILER "gcc"
#endif


int main() {
	lite3_ctx *ctx = lite3_ctx_create_with_size(1024*1024*256);
	if (!ctx) {
		perror("Failed to create context");
		return 1;
	}
	
	printf("converting doc to Lite3\n");
	if (lite3_ctx_json_dec_file(ctx, "/tmp/1.json") < 0)
		goto error;
	notify_with_pid("C/" COMPILER " (lite3_context_api)");

	lite3_iter iter;
	size_t coords_ofs;
	if (lite3_ctx_get_arr(ctx, 0, "coordinates", &coords_ofs) < 0)
		goto error;
	if (lite3_ctx_iter_create(ctx, coords_ofs, &iter) < 0)
		goto error;
	
	int count = 0;
	double sum_x = 0.0, sum_y = 0.0, sum_z = 0.0;
	double tmp_x, tmp_y, tmp_z;
	size_t ofs;
	while (lite3_ctx_iter_next(ctx, &iter, NULL, &ofs) == LITE3_ITER_ITEM) {
		if (lite3_ctx_get_f64(ctx, ofs, "x", &tmp_x) < 0
		    || lite3_ctx_get_f64(ctx, ofs, "y", &tmp_y) < 0
		    || lite3_ctx_get_f64(ctx, ofs, "z", &tmp_z) < 0) {
			goto error;
		}
		sum_x += tmp_x;
		sum_y += tmp_y;
		sum_z += tmp_z;
		++count;
	}
	const double inv = 1.0 / count;
	const double avg_x = sum_x * inv;
	const double avg_y = sum_y * inv;
	const double avg_z = sum_z * inv;

	notify("stop");
	printf("Coordinate {\"x\": %e, \"y\": %e, \"z\": %e}\n", avg_x, avg_y, avg_z);

	lite3_ctx_destroy(ctx);
	return 0;
error:
	perror("Error running benchmark");
	lite3_ctx_destroy(ctx);
	return 1;
}