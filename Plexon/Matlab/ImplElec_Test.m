%%%%%%%%%%%%%%根据各通道若干时间段spike次数及通过训练得到的回归模型计算相应拉杆动作%%%%%%%%%%%%%%%%
function vecMov =ImplElec_Test(matSpikeCount, strFileName)
%   变量说明
%	matSpikeCount整形矩阵, 各神经信号通道每时间段spike次数. 阶数为NumNeuChan*NumTimeBin (神经信号通道数*时间段数)
%   NumTimeBin必须大于等于模型文件中的nEpo
%	vecMov浮点向量,根据神经信号和回归模型预测的动作. 阶数为NumEqChan (运动感知设备通道数)
%   目前摇杆操作任务中,vecMov最大值所在通道对应的方向及为摇杆方向
%	目前认为拉杆有4个通道. 未触发为0, 触发为1. 每通道一个输入,方便以后输入设备扩展
%	strFileName，存储模型的文件名, 扩展名必须是".mat"
%   nWorkMode 工作模式, 从模型文件中读取. 1为线性模型; 2对数(输入)线性模型

%   Initializing and checking
load(strFileName);
%   variables included 'matC','nWorkMode','NumNeuChan','NumEqChan','nEpo'
if(size(matSpikeCount,2)~=nEpo)
    error('The length of signal does not match nEpo.');
end
if(min(min(matSpikeCount))<0)
    error('Negative value exists in matSpikeCount');
end

switch nWorkMode
    case 1
        matSig=matSpikeCount;
    case 2
        matSig=log(matSpikeCount+1);
    otherwise
        error('Undefined work mode');
end

%Galculate the movement
if( nWorkMode==1 ||nWorkMode==2)
    vecX=zeros(NumNeuChan*nEpo,1);
    for indEpo=1:nEpo
        vecX((indEpo-1)*NumNeuChan+1:indEpo*NumNeuChan,1)=matSig(:,1+nEpo-indEpo);
    end
    vecMov= matC*vecX; 
end


end