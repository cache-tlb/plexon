%ECOG train
function nRtn=Mat_ECOG_Train(vecStrFileName, MAXROUTES, indTriger, spRateOri, nDown, strResultFileName)
%Input:
% vecStrFileName:  由所有数据文件名构成的数组，文件中每导联（通道）为一行；
% MAXROUTES： 最大有效导联数；
% indTriger: 触发数据对应通道在文件中的行数；
% spRateOri: 采样率
% nDown: 降采样倍数
% strResultFileName: 训练结果文件名，最好在当前目录下。必须为*.mat；
% Output
% 正常情况下返回值为0，如果出错则可catch到相应异常信息

minDuration=1;
MAXFEANUM=6;
RELARSQTHRES=0.5;   %Discard features if its rsquare value is under the relative threshold

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Load Data%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
nFinishedTrainNum=length(vecStrFileName);
cellECOGData= cell(1,nFinishedTrainNum);
cellTriger= cell(1,nFinishedTrainNum);
for indTrain=1:length(vecStrFileName)
    matTmp=load(vecStrFileName{indTrain});
    cellECOGData{indTrain}=matTmp(1:MAXROUTES,:);
    cellTriger{indTrain}=matTmp(indTriger,:);
end

%%%%%%%%%%%%%%%%%%%%%%     Data format transform   %%%%%%%%%%%%%%%%%%%%%%%%
%Down sample
for indTrain=1:nFinishedTrainNum
    cellECOGData{indTrain}=downsample(cellECOGData{indTrain}',nDown)';
    cellTriger{indTrain}=downsample(cellTriger{indTrain}',nDown)';
    cellTriger{indTrain}=cellTriger{indTrain}-mean(cellTriger{indTrain});
    cellTriger{indTrain}=sign(cellTriger{indTrain});
end
spRate=round(spRateOri/nDown);

minSpPoints=minDuration*spRate;

%%%%%%%%%%%%%%%%%%%%%%     Data format transform   %%%%%%%%%%%%%%%%%%%%%%%%
matECOGDataC1=[];
matECOGDataC2=[];
vecStartPosC1=[];
vecStartPosC2=[];

for indTrain=1:nFinishedTrainNum
    indPosStart=1;
    indPosEnd=indPosStart;
    for indPos=1:length(cellTriger{indTrain})
        if( indPos<length(cellTriger{indTrain})-1 && cellTriger{indTrain}(indPos+1) ~= cellTriger{indTrain}(indPosStart) ...
                || indPos==length(cellTriger{indTrain}))
            indPosEnd=indPos;
            if(indPosEnd+1-indPosStart>minDuration*spRate)       %Add data to class data if the duration is long enough
                if(cellTriger{indTrain}(indPosStart)==-1)
                    vecStartPosC1=[vecStartPosC1 size(matECOGDataC1,2)+1];
                    matECOGDataC1=[matECOGDataC1 cellECOGData{indTrain}(:,indPosStart:indPosEnd)];
                else
                    vecStartPosC2=[vecStartPosC2 size(matECOGDataC2,2)+1];
                    matECOGDataC2=[matECOGDataC2 cellECOGData{indTrain}(:,indPosStart:indPosEnd)];   
                end                
            end
            indPosStart=indPos+1;
        end
    end
end
NumTrialC1=length(vecStartPosC1);
NumTrialC2=length(vecStartPosC2);
vecEndPosC1=[vecStartPosC1(2:end)-1 size(matECOGDataC1,2)];
vecEndPosC2=[vecStartPosC2(2:end)-1 size(matECOGDataC2,2)];

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Preprocessing%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%   Remove DC %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
for indTrial=1:NumTrialC1
    nSampleLen=vecEndPosC1(indTrial)-vecStartPosC1(indTrial)+1;
    matECOGDataC1(:,vecStartPosC1(indTrial):vecEndPosC1(indTrial))= ...
        matECOGDataC1(:,vecStartPosC1(indTrial):vecEndPosC1(indTrial)) ...
        -repmat(mean(matECOGDataC1(:,vecStartPosC1(indTrial):vecEndPosC1(indTrial)),2),[1,nSampleLen]);
end
    
for indTrial=1:NumTrialC2
    nSampleLen=vecEndPosC2(indTrial)-vecStartPosC2(indTrial)+1;
    matECOGDataC2(:,vecStartPosC2(indTrial):vecEndPosC2(indTrial))= ...
        matECOGDataC2(:,vecStartPosC2(indTrial):vecEndPosC2(indTrial)) ...
        -repmat(mean(matECOGDataC2(:,vecStartPosC2(indTrial):vecEndPosC2(indTrial)),2),[1,nSampleLen]);
end

% %%%%%%%%%%%%%%%%%%%%%   Spatial Filter %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
matECOGDataC1=matECOGDataC1-repmat(mean(matECOGDataC1,1),[MAXROUTES,1]);
matECOGDataC2=matECOGDataC2-repmat(mean(matECOGDataC2,1),[MAXROUTES,1]);

%%%%%%%%%%%%%%%%%%%%%   Transform to the frequency domain %%%%%%%%%%%%%%%%%
FFTTime=0.5;
NumFFTLen=FFTTime*spRate;
baseFre=1/FFTTime;
MINFRE=6;
MINFREIND=round(MINFRE/baseFre)+1;
MAXFRE=500;
MAXFRENUM=round(MAXFRE/baseFre)+1;
nModelOrder=28;
fBinWidth=baseFre;
nEvaluationsPerBin=10;
nDetrendOption=1;
nSampleBlockSize=166;
nWinLenInBlock=2;
nLogMode=1;         %1 for log processing
nTrialMeanMode=1;   %1 for take trial mean
nSpecTransMode=0;   %0 for FFT, 1 for AR
paraAR=[nModelOrder,fBinWidth,MAXFRENUM*fBinWidth,fBinWidth,nEvaluationsPerBin,nDetrendOption,spRate,nSampleBlockSize,nWinLenInBlock];

cellRouteFreDataC1=cell(MAXROUTES,MAXFRENUM);
for indTrial=1:NumTrialC1
    if nSpecTransMode==0        %FFT
        for indRoute=1:MAXROUTES        
            [matTmpFreData,vecF,vecT]=spectrogram(matECOGDataC1(indRoute,vecStartPosC1(indTrial):vecEndPosC1(indTrial)),hamming(NumFFTLen),round(NumFFTLen*0.9),NumFFTLen,spRate);
            for indFre=1:MAXFRENUM
                if (nLogMode==1)
                    cellRouteFreDataC1{indRoute,indFre}=[cellRouteFreDataC1{indRoute,indFre} mean(log(abs(matTmpFreData(indFre,:))))];
                else
                    cellRouteFreDataC1{indRoute,indFre}=[cellRouteFreDataC1{indRoute,indFre} mean(abs(matTmpFreData(indFre,:)))];
                end  
            end
        end
    elseif nSpecTransMode==1    %AR
        [mat3TmpFreData, vecF] = mem( matECOGDataC1(:,vecStartPosC1(indTrial):vecEndPosC1(indTrial))', paraAR);
        for indRoute=1:MAXROUTES
            for indFre=1:MAXFRENUM
                if (nLogMode==1)
                    cellRouteFreDataC1{indRoute,indFre}=[cellRouteFreDataC1{indRoute,indFre} mean(log(abs(mat3TmpFreData(indFre,indRoute,:))))];
                else
                    cellRouteFreDataC1{indRoute,indFre}=[cellRouteFreDataC1{indRoute,indFre} mean(abs(mat3TmpFreData(indFre,indRoute,:)))];
                end
            end       
        end
    end
end


cellRouteFreDataC2=cell(MAXROUTES,MAXFRENUM);
for indTrial=1:NumTrialC2
    if nSpecTransMode==0        %FFT
        for indRoute=1:MAXROUTES        
            [matTmpFreData,vecF,vecT]=spectrogram(matECOGDataC2(indRoute,vecStartPosC2(indTrial):vecEndPosC2(indTrial)),hamming(NumFFTLen),round(NumFFTLen*0.9),NumFFTLen,spRate);
            for indFre=1:MAXFRENUM
                if (nLogMode==1)
                    cellRouteFreDataC2{indRoute,indFre}=[cellRouteFreDataC2{indRoute,indFre} mean(log(abs(matTmpFreData(indFre,:))))];
                else
                    cellRouteFreDataC2{indRoute,indFre}=[cellRouteFreDataC2{indRoute,indFre} mean(abs(matTmpFreData(indFre,:)))];
                end
            end
        end
    elseif nSpecTransMode==1    %AR
        [mat3TmpFreData, vecF] = mem( matECOGDataC2(:,vecStartPosC2(indTrial):vecEndPosC2(indTrial))', paraAR);
        for indRoute=1:MAXROUTES
            for indFre=1:MAXFRENUM
                if (nLogMode==1)
                    cellRouteFreDataC2{indRoute,indFre}=[cellRouteFreDataC2{indRoute,indFre} mean(log(abs(mat3TmpFreData(indFre,indRoute,:))))];
                else
                    cellRouteFreDataC2{indRoute,indFre}=[cellRouteFreDataC2{indRoute,indFre} mean(abs(mat3TmpFreData(indFre,indRoute,:)))];
                end
            end       
        end
    end
end


%%%%%%%%%%%%%%%%%%%%%   Compute rsq %%%%%%%%%%%%%%%%%
matRSq=zeros(MAXROUTES,MAXFRENUM);
for indRoute=1:MAXROUTES
    for indFre=1:MAXFRENUM
        matRSq(indRoute,indFre)=rsqu(cellRouteFreDataC1{indRoute,indFre},cellRouteFreDataC2{indRoute,indFre});       
    end
end



%Select features
NEIGHBORDIST=4/baseFre;     %Neighbour freqs in the same channel can not be chosen as feature both.
matIndFea=zeros(MAXFEANUM,2);
vecRSqFea=zeros(MAXFEANUM,1);
for indRoute=1:MAXROUTES
    for indFre=MINFREIND:MAXFRENUM
        nChanged=0;
        if matRSq(indRoute,indFre)>vecRSqFea(MAXFEANUM)
            nNeighborSel=0;
            for indFeaTmp=1:MAXFEANUM
                if(matIndFea(indFeaTmp,1)==indRoute && abs(matIndFea(indFeaTmp,2)-indFre)<=NEIGHBORDIST)
                    nNeighborSel=1;
                    if matRSq(indRoute,indFre)> vecRSqFea(indFeaTmp)
                        vecRSqFea(indFeaTmp)=matRSq(indRoute,indFre);
                        matIndFea(indFeaTmp,2)=indFre;
                        nChanged=1;
                    end
                end
            end
            if(nNeighborSel~=1)
               vecRSqFea(MAXFEANUM)=matRSq(indRoute,indFre);
               matIndFea(MAXFEANUM,:)=[indRoute,indFre];
               nChanged=1;
            end
            if nChanged==1
                [vecRSqFea, indRSqFea]=sort(vecRSqFea,'descend');
                matIndFea=matIndFea(indRSqFea,:);
            end
        end
    end
end

for indFea=1:MAXFEANUM
    if vecRSqFea(indFea)<RELARSQTHRES*vecRSqFea(1)
        vecRSqFea=vecRSqFea(1:indFea-1);
        matIndFea=matIndFea(1:indFea-1,:);
        break;
    end
end

%Generate features
numFea=length(vecRSqFea);
numSamp1=length(cellRouteFreDataC1{1,1});
numSamp2=length(cellRouteFreDataC2{1,1});
numSamp=numSamp1+numSamp2;
vecLabel1=-1*ones(numSamp1,1);
vecLabel2=ones(numSamp2,1);
vecLabelTrain=[vecLabel1',vecLabel2']';
matFeaTrain=zeros(numSamp,numFea);

for indFea=1:numFea
    matFeaTrain(1:numSamp1,indFea)=cellRouteFreDataC1{matIndFea(indFea,1),matIndFea(indFea,2)};
    matFeaTrain(numSamp1+1:numSamp,indFea)=cellRouteFreDataC2{matIndFea(indFea,1),matIndFea(indFea,2)};    
end

%%%%%%%%%%%%%%% Train the classifier %%%%%%%%%%%%%%%%%%%%%%%%
svm_model = svmtrain(vecLabelTrain,matFeaTrain,'-t 0');
% save the status
save(strResultFileName,'svm_model','matIndFea','MAXROUTES','spRateOri','nDown','MAXFRE','-mat');

end