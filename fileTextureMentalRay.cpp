/* The mental ray version of fileTexture sample
   To use one shader network with both mental ray and Maya nodes, you need to provide both mental ray version and 
   Maya version with the same name.

   Copy compiled fileTexture.dll file to the shader folder of mental ray, and fileTexture.mi to shader/include folder.
   Notice that the .mi file must use ASCII encoding, or mental ray will refuse to load the shader.
*/

#include <shader.h>
#include <memory.h>
#if defined(__cplusplus)
extern "C" {
#endif

	// Macro for testing whether plug is connected to another node
#define MAYA_CONNECTED(p) \
	(mi_has_connections(state) && \
	!mi_is_nonshader_tag(mi_get_ghost(state,p)))

	// Define fileTexture input
	typedef struct fileTexture_Paras {
		miTag fileName;
		miVector uvCoord;
	} fileTexture_Paras;

	// Define fileTexture result
	typedef struct fileTexture_Results {
		miColor outColor;
		miScalar outAlpha;
	} fileTexture_Results;

	// Define userdata
	typedef struct fileTexture_Userdata	{
		miImg_file   ifp;
		miImg_image *image;
	} fileTexture_Userdata;

	// Version
	DLLEXPORT int fileTexture_version(void) {
		return(1);
	}

	// For more reference about initialization, please visit http://docs.autodesk.com/MENTALRAY/2015/ENU/mental-ray-help/
	// To improve the performance, open the file during the initialization	
	DLLEXPORT void fileTexture_init(  miState         *state, 
								 	  struct fileTexture_Paras *paras,     // Our parameters
									  miBoolean       *inst_req)  // need instance init?
	{
		if (!paras) {               // first without paras
			*inst_req = miTRUE;     // We have parameters for our instances, so we want instance init.

		} else {                    // para is not null, instance init.
			void **user;
			miTag imageTag = 0;
			fileTexture_Userdata *data = 0;

			mi_query(miQ_FUNC_USERPTR, state, 0, &user);
			*user = mi_mem_allocate(sizeof(fileTexture_Userdata));
			if(*user != NULL)
			{
				data = (fileTexture_Userdata *)*user;

				memset(data, 0, sizeof(fileTexture_Userdata));
				// Try to get image filename
				imageTag = *mi_eval_tag(&paras->fileName);
				char *fileName = (char*)mi_db_access(imageTag);

				// Open and read image.
				if (!(mi_img_open(&(data->ifp),
					fileName,
					miIMG_TYPE_RGBA)))
					return;
				if (!(data->image = mi_img_image_alloc(&(data->ifp)))) {
					mi_img_close(&(data->ifp));
					return;
				}
				if (!(mi_img_image_read(&(data->ifp), data->image))) {
					mi_mem_release(data->image);
					mi_img_close(&(data->ifp));
					return;
				}
			}
		}
	}

	// Close the file and clean up during the exit.
	DLLEXPORT void fileTexture_exit(  miState         *state,     
									  struct myshader *paras)     // for instance 
	{
		if (!paras) {              // the final step
			
		} else {                    // shader instance exit.
			// Get user data and close the file
			void **user;                           
			mi_query(miQ_FUNC_USERPTR, state, 0, &user);
			if(*user != NULL)
			{							
				fileTexture_Userdata* data = (fileTexture_Userdata*)*user;
				if(data->image != NULL)
				{
					mi_mem_release(data->image);
					mi_img_close(&(data->ifp));
				}						
				mi_mem_release(*user);        
			}
		}
	}

	// fileTexture Shader
	DLLEXPORT miBoolean fileTexture ( fileTexture_Results *result,
									  miState	*state,
									  fileTexture_Paras *paras) {
			miImg_image *image = 0;
			void **user;
			fileTexture_Paras *data = 0;

			// Get coord to read from image
			miVector coord = (MAYA_CONNECTED(&paras->uvCoord)) ?
				*mi_eval_vector(&paras->uvCoord) :
				state->tex_list[0];

			// Get user data
			mi_query(miQ_FUNC_USERPTR, state, 0, &user);
			if(*user != NULL)
			{
				fileTexture_Userdata* data = (fileTexture_Userdata*)*user;
				image = data->image;				
				// If image opened successful, read image 
				if (image)				
				{
					if (coord.x < 0.0f || coord.x > 1.0f)
						coord.x -= floorf(coord.x);
					if (coord.y < 0.0f || coord.y > 1.0f)
						coord.y -= floorf(coord.y);
					int width = mi_img_get_width(image);
					int height = mi_img_get_height(image);

					mi_img_get_color(
						image, &result->outColor,
						(int)(width*coord.x),
						(int)(height*coord.y));

					result->outAlpha = result->outColor.a;

					return miTRUE;
				}
			}

			// Default routine, output black
			result->outColor.a = result->outColor.r = result->outColor.g = result->outColor.b = 0.0f;
			result->outColor.a = result->outAlpha = 1.0f;
			return miFALSE;
	}
#if defined(__cplusplus)
}
#endif