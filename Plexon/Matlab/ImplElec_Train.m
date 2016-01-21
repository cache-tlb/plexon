%%%%%%%%%%%%%%根据各通道每时间段spike次数及相应拉杆动作建立回归模型%%%%%%%%%%%%%%%%
function fErr =ImplElec_Train(matSpikeCount,matMov,nEpo,strFileName,nWorkMode)
%   变量说明
%	matSpikeCount整形矩阵, 各神经信号通道每时间段spike次数. 阶数为NumNeuChan*NumTimeBin (神经信号通道数*时间段数)
%	matMov整形矩阵,每时间段从运动感知设备处获取的动作. 阶数为NumEqChan*NumTimeBin (运动感知设备通道数*时间段数)
%	nEpo参与拟合的信号时间段数. 及从当前时间段nTNow到nTNow-nEpo+1时间段
%	目前认为拉杆有5个通道. 分别为上、下、左、右、停. 未触发为0, 触发为1. 每通道一个输入,方便以后输入设备扩展
%	strFileName，存储模型的文件名, 扩展名必须是".mat"
%   nWorkMode 工作模式. 1为线性模型; 2对数(输入)线性模型

%   Initializing and checking
[NumNeuChan,NumTimeBin]= size(matSpikeCount);
[NumEqChan,NumTimeBinTmp]= size(matMov);

if(NumTimeBin~=NumTimeBinTmp)
    error('The length of signal and movments do not match.');
end
if(NumEqChan*NumTimeBin<nEpo*NumNeuChan)
    error('NumEqChan*NumTimeBin<nEpo*NumNeuChan, not enough information to fit.');
end
if(min(min(matSpikeCount))<0)
    error('Negative value exists in matSpikeCount');
end
if(max(max(matMov))>1 || min(min(matMov))<0)
    error('Matcov should be between 0 and 1');
end

switch nWorkMode
    case 1
        matSig=matSpikeCount;
    case 2
        matSig=log(matSpikeCount+1);
    otherwise
        error('Undefined work mode');
end


%Galculate the model
if( nWorkMode==1 ||nWorkMode==2)
    matX=zeros(NumNeuChan*nEpo,NumTimeBin-nEpo+1);
    matY=matMov(:,nEpo:end);
    for indData=1:NumTimeBin-nEpo+1
        for indEpo=1:nEpo
            matX((indEpo-1)*NumNeuChan+1:indEpo*NumNeuChan,indData)=matSig(:,indData+nEpo-indEpo);
        end
    end
    
    %Calculate the coefficients
    matC= matY*matX'/(matX*matX');
    %Calculate the residual
    matQ= matY-matC*matX;    
end
    

%Save the model file

save(strFileName,'matC','nWorkMode','NumNeuChan','NumEqChan','nEpo');
fErr= mean(mean(matQ.^2));
end