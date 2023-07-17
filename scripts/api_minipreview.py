import cv2
from PIL import Image
import base64
import numpy as np
from modules import shared
from fastapi import FastAPI, Query, Request, UploadFile
from fastapi.responses import JSONResponse
from fastapi.encoders import jsonable_encoder
from fastapi.exceptions import RequestValidationError

def minip_api(_, app: FastAPI):
    print("Loading mini preview API endpoints")

    @app.exception_handler(RequestValidationError)
    async def validation_exception_handler(request: Request, exc: RequestValidationError):
        return JSONResponse(
            status_code=422,
            content=jsonable_encoder({"detail": exc.errors(), "body": exc.body}),
        )

    @app.get("/minip/api_version")
    async def minip_api_version():
        return JSONResponse(content={"version": '1.0'})

    @app.get("/minip/version")
    async def minip_version():
        return JSONResponse(content={"version": '0.0'})

    @app.get("/minip/run")
    async def minip_run():
        if shared.state.job_count == 0:
            progress=0
        else:

            progress = 0.01

            if shared.state.job_count > 0:
                progress += shared.state.job_no / shared.state.job_count
            if shared.state.sampling_steps > 0:
                progress += 1 / shared.state.job_count * shared.state.sampling_step / shared.state.sampling_steps

            progress = round(min(progress, 1)*100)

        if shared.state.current_image:
            img = shared.state.current_image.convert('RGB')

            size = img.size
            img = cv2.cvtColor(np.array(shared.state.current_image), cv2.COLOR_BGR2RGB)
            if size[0]/320 > size[1]/240:
                size = (320, int(size[1]*320/size[0]))
            else:
                size = (int(size[0]*240/size[1]), 240)
            img = cv2.resize(img, size, interpolation = cv2.INTER_AREA)
            img = cv2.imencode('.jpg', img, [int(cv2.IMWRITE_JPEG_QUALITY), 30])[1]
            img = base64.b64encode(img)
        else:
            img = None

        return JSONResponse(content=jsonable_encoder({"progress": progress, "current_image": img}))

try:
    import modules.script_callbacks as script_callbacks

    script_callbacks.on_app_started(minip_api)
    print("SD-Webui minp API layer loaded")
except ImportError:
    print("Unable to import script callbacks. minip")

