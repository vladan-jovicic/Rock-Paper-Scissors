#include "Kinect.h"
#include <queue>
#include <cmath>
#include <stack>
#include <vector>
#include <algorithm>

#define PI 3.14159265

void msg(char *p){
	printf("%s\n",p);
}

pixel			comparablePixel;
int				matrix[480][640] = {0};

bool belong(int x, int y, pixel handP){
	pixel upper, down;
	upper = pixel(0,0);

	if(handP.x - 50 >= 0)upper.x=handP.x-50;
	if(handP.y - 50 >= 0)upper.y=handP.y-50;

	down = pixel(639,479);

	if(handP.x + 50 < 640)down.x=handP.x+50;
	if(handP.y + 50 < 480)down.y=handP.y+50;

	if(x >= upper.x && x <= down.x){
		if(y>=upper.y && y<=down.y){
			return true;
		}
	}
	return false;
}

int udaljenost(pixel a, pixel b){
	return (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y);
}


int cmp(const void *_a, const void *_b){
	int dist1,dist2;
	pixel *a = (pixel *)_a;
	pixel *b = (pixel *)_b;

	dist1 = (comparablePixel.x - a->x)*(comparablePixel.x - a->x) + (comparablePixel.y - a->y)*(comparablePixel.x - a->x);
	dist2 = (comparablePixel.x - b->x)*(comparablePixel.x - b->x) + (comparablePixel.y - b->y)*(comparablePixel.x - b->x);
	return dist1 - dist2;
}

Kinect::Kinect(int ind){
	index = ind;
}

bool Kinect::initializeKinect(DWORD dwFlags){
	HRESULT res = S_OK;
	if(NuiCreateSensorByIndex(index,&sensor) < 0) {
		msg("failed to initialize by index");
		return false;
	}
	res = sensor->NuiInitialize(dwFlags);
	if(FAILED(res)){
		msg("Failed to initialize");
		return false;
	}
	return true;
}

bool Kinect::openImageStream(NUI_IMAGE_RESOLUTION resolution){
	HRESULT res;
	freeEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	res = sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR,resolution,0,2,freeEvent,&rgbCamera);
	if(FAILED(res)){
		msg("failed to open stream");
		return false;
	}
	return true;
}

bool Kinect::openDepthStream(NUI_IMAGE_RESOLUTION resolution){
	HRESULT res;
	depthEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	res = sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH,resolution,0,2,depthEvent,&depthStream);
	if(FAILED(res)){
		msg("failed to open depth stream");
		return false;
	}
	return true;
}

bool Kinect::getRGBImage(IplImage *img){
	HRESULT res;
	NUI_IMAGE_FRAME imageFrame;
	res = sensor->NuiImageStreamGetNextFrame(rgbCamera,10,&imageFrame);
	if(FAILED(res)){
		msg("failed to get next frame");
		return false;
	}
	INuiFrameTexture *texture = imageFrame.pFrameTexture;
	NUI_LOCKED_RECT lockedRect;
	res = texture->LockRect(0,&lockedRect,NULL,0);
	if(FAILED(res)){
		msg("failed to lock rect");
		return false;
	}
	if(lockedRect.Pitch != 0){
		BYTE *buffer = (BYTE *)lockedRect.pBits;
		cvSetData(img,buffer,lockedRect.Pitch);
		//cvCvtColor(img,img,CV_RGB2HSV);
	}else {
		return false;
	}
	texture->UnlockRect(0);
	res = sensor->NuiImageStreamReleaseFrame(rgbCamera,&imageFrame);
	return true;
}

bool Kinect::getDepthImage(IplImage *img, int *numOfFingers){
	HRESULT						res;
	BOOL						nearM;
	NUI_IMAGE_FRAME				imageFrame;
	INuiFrameTexture			*texture;
	NUI_LOCKED_RECT				lockedRect;

	res = sensor->NuiImageStreamGetNextFrame(depthStream,100,&imageFrame);
	if(FAILED(res)){
		msg("failed to get next depth frame");
		return false;
	}
	// = imageFrame.pFrameTexture;
	res = sensor->NuiImageFrameGetDepthImagePixelFrameTexture(depthStream,&imageFrame,&nearM,&texture);
	if(FAILED(res)){
		sensor->NuiImageStreamReleaseFrame(depthStream,&imageFrame);
		msg("failed to get depth texture");
		return false;
	}
	
	res = texture->LockRect(0,&lockedRect,NULL,0);
	if(FAILED(res)){
		sensor->NuiImageStreamReleaseFrame(depthStream,&imageFrame);
		msg("failed to lock rect");
		return false;
	}

	if(lockedRect.Pitch!=0){
		bool failed = false;
		if(FAILED(res)){
			failed = true;
		}

		bool			visited[480][640]={0};
		int				dx[] = {1, 1, 1, -1, -1, -1, 0, 0};
		int				dy[] = {1, 0, -1, 1, 0, -1, 1, -1};
		queue<pixel>	q;
		pixel			*contour = new pixel[5000];
		pixel			*prsti = new pixel[100];
		int				sizeOfCont = 0;
		Vector4			p;
		float			hx, hy;
		bool			isOk = false;
		int				minX = 640, minY = 480, maxX = -1, maxY = -1;
		int				ind = -5,sizeOfSort, sizeOfPrst = 0;

		if(getRightHand(&p)){

			const NUI_DEPTH_IMAGE_PIXEL		*pBuff = reinterpret_cast<const NUI_DEPTH_IMAGE_PIXEL *>(lockedRect.pBits);
			int								minDepth = (nearM ? NUI_IMAGE_DEPTH_MINIMUM_NEAR_MODE : NUI_IMAGE_DEPTH_MINIMUM) >> NUI_IMAGE_PLAYER_INDEX_SHIFT;
			int								maxDepth = (nearM ? NUI_IMAGE_DEPTH_MAXIMUM_NEAR_MODE : NUI_IMAGE_DEPTH_MAXIMUM) >> NUI_IMAGE_PLAYER_INDEX_SHIFT;
			
			BYTE *buff = tmp;
			//isOk = true;

			NuiTransformSkeletonToDepthImage(p,&hx,&hy,NUI_IMAGE_RESOLUTION_640x480);
			pixel handP = pixel((int)(floor(hx+0.5)), (int)(floor(hy+0.5)) );

			if(handP.x >= 0 && handP.x < 640 && handP.y >=0 && handP.y < 480){

				isOk = true;
				q.push(handP);
				USHORT compPixel = (pBuff+handP.y*640 + handP.x)->depth;
				cvSet(img,cvScalar(0,0,0));
				memset(matrix,0,sizeof(matrix));
				for(int i=max(0,handP.y-50);i<min(480,handP.y+50);i++){
					for(int j=max(0,handP.x-50);j<min(640,handP.x+50);j++){
						USHORT currPixel = (pBuff + i*640 + j)->depth;
						if(abs(currPixel - compPixel) < 70 ){
							matrix[i][j] = 1;
							*(buff + i*img->widthStep + 4*j) = 255;
							*(buff + i*img->widthStep + 4*j + 1) = 0;
							*(buff + i*img->widthStep + 4*j + 2) = 0;
							minX = min(minX, j);
							maxX = max(maxX,j);
							minY = min(minY,i);
							maxY = max(maxY,i);
						}
					}
				}
				for(int i=max(0,handP.y-50);i<min(480,handP.y+50);i++){
					for(int j=max(0,handP.x-50);j<min(640,handP.x+50);j++){
						if(matrix[i][j] == 0) {
							for(int k=0;k<8;k++){
								pixel next = pixel(j + dx[k], i+dy[k]);
								if(next.x < 0 || next.x >= 640 || next.y < 0 || next.y >= 480)continue;
								if(matrix[next.y][next.x] == 1){
									matrix[i][j]= 2;
									contour[sizeOfCont++] = pixel(j,i);
									*(buff + i*img->widthStep + 4*j) = 0;
									*(buff + i*img->widthStep + 4*j+1) = 255;
									*(buff + i*img->widthStep + 4*j+2) = 0;
								}
							}
						}
						if(i == minY && matrix[i][j] == 1){
							matrix[i][j] = 2;
							contour[sizeOfCont++] = pixel(j,i);
							*(buff + i*img->widthStep + 4*j) = 0;
							*(buff + i*img->widthStep + 4*j+1) = 255;
							*(buff + i*img->widthStep + 4*j+2) = 0;
						}
						if(i == maxY && matrix[i][j] == 1){
							contour[sizeOfCont++] = pixel(j,i);
							matrix[i][j] = 2;
							*(buff + i*img->widthStep + 4*j) = 0;
							*(buff + i*img->widthStep + 4*j+1) = 255;
							*(buff + i*img->widthStep + 4*j+2) = 0;
						}
						if(j == maxX && matrix[i][j] == 1){
							contour[sizeOfCont++] = pixel(j,i);
							matrix[i][j] = 2;
							*(buff + i*img->widthStep + 4*j) = 0;
							*(buff + i*img->widthStep + 4*j+1) = 255;
							*(buff + i*img->widthStep + 4*j+2) = 0;
						}
						if(j == minX && matrix[i][j] == 1){
							contour[sizeOfCont++] = pixel(j,i);
							matrix[i][j] = 2;
							*(buff + i*img->widthStep + 4*j) = 0;
							*(buff + i*img->widthStep + 4*j+1) = 255;
							*(buff + i*img->widthStep + 4*j+2) = 0;
						}
					}
				} // kraj for petlje
			} // kraj ifa za piksele ruke



			if(sizeOfCont < 100)isOk = false;
			vector<pixel>		sorted;
			bool				generalF = false;

			if(isOk){

				
				
				sorted.push_back(contour[0]);
				memset(visited,0,sizeof(visited));

				while(!sorted.empty()){
					pixel		top = sorted[sorted.size() - 1];
					bool		fnd = false;

					for(int i=0;i<8;i++){
						pixel	next = pixel(top.x + dx[i], top.y + dy[i]);
						if(next.x < 0|| next.x >= 640 || next.y < 0 || next.y >=480)continue;
						if(visited[next.y][next.x])continue;

						if(matrix[next.y][next.x] == 2 ){
							fnd = true;
							sorted.push_back(next);
							visited[next.y][next.x] = true;
							if(next.x == contour[0].x && next.y == contour[0].y && sorted.size() > 100){
								generalF = true;
							}
							break;
						}
					}
					if(generalF)break;
					if(!fnd) {
						sorted.pop_back();
					}
				}
			}

			cvSetData(img,tmp,img->widthStep);

			sizeOfSort = sorted.size();

			//detekcija prstiju

			if(isOk){
				//cvRectangle(img,cvPoint(minX,minY),cvPoint(maxX,maxY),CV_RGB(255,0,0),2,8,0);
				/*for(int i = 1;i<sizeOfFsort;i++){
					cvLine(img,cvPoint(fsorted[i-1].x,fsorted[i-1].y),cvPoint(fsorted[i].x,fsorted[i].y),CV_RGB(255,0,0),1,8,0);
				}*/
				for(int i=0;i<sizeOfSort;i++){
					int			ri = (i-22)%sizeOfSort;
					int			ir = (i+22)%sizeOfSort;

					if(ri<0)ri+=sizeOfSort;
					
					int			x1 = sorted[ri].x - sorted[i].x;
					int			y1 = sorted[i].y - sorted[ri].y;
					int			x2 = sorted[ir].x - sorted[i].x;
					int			y2 = sorted[i].y - sorted[ir].y;

					double		cosalfa = (double)(x1*x2 + y1*y2)/(double)( sqrt((double)(x1*x1+y1*y1))*sqrt((double)(x2*x2+y2*y2)) );
					int			degree = acos(cosalfa) * 180.0 / PI;

					if( abs(degree-40)<=5){
						//to je kandidat za prstic
						if(udaljenost(handP,sorted[ri]) > udaljenost(handP,sorted[i]) || udaljenost(handP,sorted[ir]) > udaljenost(handP,sorted[i]) ){
						}else {
							prsti[sizeOfPrst++] = sorted[i];
							cvCircle(img,cvPoint(sorted[i].x,sorted[i].y),1,CV_RGB(255,255,255),3,8,0);
							i+=25;
						}
					}

					//gesture recogniion
					
					if(sizeOfSort == 0 ){
						*numOfFingers = -1;
					}

				}
				if(sizeOfSort != 0){
					*numOfFingers = sizeOfPrst;
				}
				//printf("%d\n",sizeOfPrst);
			}else {
				return false;
			}
			
		}
	}else {
		return false;
	}
	texture->UnlockRect(0);
	res = sensor->NuiImageStreamReleaseFrame(depthStream,&imageFrame);
	return true;
}

void Kinect::shutdown(){
	sensor->Release();
	sensor->NuiShutdown();
	CloseHandle(freeEvent);
	CloseHandle(depthEvent);
}

bool Kinect::getRightHand(Vector4 *p){
	HRESULT res;
	NUI_SKELETON_FRAME skeletonFrame;
	bool nadjen = false;
	res = sensor->NuiSkeletonGetNextFrame(0,&skeletonFrame);
	//Vector4 p;
	if(FAILED(res)){
		msg("can not get skeleton frame");
		return false;
	}
	for(int i=0;i<6;i++){
		if(skeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED){
			*p = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT];
			nadjen = true;
		}
	}
	return nadjen;
}
bool Kinect::getCoordinatesRightHand(pixel *p){
	HRESULT					res;
	NUI_SKELETON_FRAME		skeletonFrame;
	float					hx,hy;
	bool					found=false;
	res = sensor->NuiSkeletonGetNextFrame(0,&skeletonFrame);
	Vector4 p1;
	if(FAILED(res)){
		return false;
	}
	for(int i=0;i<6;i++){
		if(skeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED){

			p1 = skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT];
			found = true;
		}
	}
	if(found){
		NuiTransformSkeletonToDepthImage(p1,&hx,&hy,NUI_IMAGE_RESOLUTION_640x480);
		*p = pixel((int)(floor(hx+0.5)), (int)(floor(hy+0.5)) );
	}
	return found;
}

