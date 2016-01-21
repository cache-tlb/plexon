%ECOG test
function nLabel=Mat_ECOG_Test(matECOGData, strFileName)
% matECOGData:  ECOG数据，每导联（通道）为一行,必须为MAXROUTES行；
% strFileName: 训练结果文件名，最好在当前目录下。必须为*.mat；
load(strFileName);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Preprocessing%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if( size(matECOGData,1) ~=MAXROUTES)
    error('The rows of matECOGData is not equal to MAXROUTES, error');
end

%Down sample
matECOGData=downsample(matECOGData',nDown)';
nSampleLen=size(matECOGData,2);
spRate=round(spRateOri/nDown);
% Remove DC
matECOGData=matECOGData-repmat(mean(matECOGData,2),[1,nSampleLen]);
% Spatial Filter
matECOGData=matECOGData-repmat(mean(matECOGData,1),[MAXROUTES,1]);

%%%%%%%%%%%%%%%%%%%%%   Transform to the frequency domain %%%%%%%%%%%%%%%%%
FFTTime=0.5;
NumFFTLen=FFTTime*spRate;
baseFre=1/FFTTime;
MINFRE=6;
MINFREIND=round(MINFRE/baseFre)+1;
MAXFRENUM=round(MAXFRE/baseFre)+1;
nLogMode=1;         %1 for log processing
nTrialMeanMode=1;   %1 for take trial mean

cellRouteFreData=cell(MAXROUTES,MAXFRENUM);
for indRoute=1:MAXROUTES        
    [matTmpFreData,vecF,vecT]=spectrogram(matECOGData(indRoute,:),hamming(NumFFTLen),round(NumFFTLen*0.9),NumFFTLen,spRate);
    for indFre=1:MAXFRENUM
        if (nLogMode==1)
            cellRouteFreData{indRoute,indFre}=[cellRouteFreData{indRoute,indFre} mean(log(abs(matTmpFreData(indFre,:))))];
        else
            cellRouteFreData{indRoute,indFre}=[cellRouteFreData{indRoute,indFre} mean(abs(matTmpFreData(indFre,:)))];
        end  
    end
end

%Generate features
numFea=size(matIndFea,1);
matFeaTest=zeros(1,numFea);
for indFea=1:numFea
    matFeaTest(1,indFea)=cellRouteFreData{matIndFea(indFea,1),matIndFea(indFea,2)};  
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%Classify%%%%%%%%%%%%%%%%%%%%%%%%%%%%
vecTmpLabel=-1;
[vecPredictLabel,svm_accu,vecDecVal] = svmpredict(vecTmpLabel,matFeaTest,svm_model);
vecDecVal
nLabel=-mean(vecDecVal);

end