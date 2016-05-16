function [cmp,eqv,idx,vec] = equivcircuit(X,ser,rng,nmc,rec)
% Select parallel/series equivalent-circuit components to best match given values.
%
% (c) 2014 Stephen Cobeldick
%
% ### Function ###
%
% Uses an exhaustive search to find the resistor/capacitor/inductor component
% value combination that, when used in a parallel or series circuit, has an
% equivalent value that is the closest match to the input numeric value.
%
% Syntax:
%  cmp = equivcircuit(X,ser,rng,nmc,rec)
%  [cmp,eqv,idx,vec] = equivcircuit(...)
%
% Matrix <cmp> has component values from an IEC 60063 E-series of values (as
% selected by input <ser>), so that equivalent circuit values calculated using
% the chosen summation method are the closest possible to the values of <X>.
%
% Any <ser> supported by "round60063" may be used to select the component
% series: E3, E6, E12, E24, E48, E96, and E192, as defined by IEC 60063.
% The permitted component value range is defined by input <rng>, while the
% number of components in the circuit is selected using the input <nmc>.
%
% Note 1: It is up to the user to account for tolerances, ratings, etc.
% Note 2: A large range + many components + high E-series = very slow!
% Note 3: Requires M-file "round60063", available on MATLAB's File Exchange.
%
% See also ROUND60063 ROUND2SF ROUND2DP SIPRE SINUM BIPRE BINUM HISTC ISFINITE SUM BSXFUN
%
% ### Parallel or Series Circuit (Input <rec>) ###
%
% Different circuits require different summation methods. Use the table below
% to look up what kind of circuit you need (e.g. parallel inductors), and then
% pick the appropriate value for the input <rec>:
%
% <rec>  | Sum of component  | For series circuit of | For parallel circuit of
% -------|-------------------|-----------------------|------------------------
% true   | value reciprocals | capacitors            | resistors / inductors
% -------|-------------------|-----------------------|------------------------
% false  | values            | resistors / inductors | capacitors
% -------|-------------------|-----------------------|------------------------
%
% ### Examples ###
%
% % Circuit: two E6 series components, sum value reciprocals:
% [cmp,eqv] = equivcircuit(666, 'E6', [500,50000], 2, true)
%  cmp = [33000; 680]
%  eqv = 666.27
%
% % Circuit: three E12 series components, sum values:
% [cmp,eqv,idx,vec] = equivcircuit(12345, 'E12', [10,20000], 3, false)
%  cmp = [12000; 330; 15]
%  eqv = 12345
%  idx = [38; 19; 3]
%  vec = [10; 12; 15; 18; 22; 27; 33; 39; 47;...12000; 15000; 18000; 22000]
%
% % Circuit: two E6 series components, sum value reciprocals:
% [cmp,eqv] = equivcircuit([8, 88, 888; 0.99, 11, Inf], 'E6', [1,5000], 2, true)
%  cmp(:,:,1) = [  47, 100;
%                  10,   1]
%  cmp(:,:,2) = [ 680,  22;
%                 100,  22]
%  cmp(:,:,3) = [2200, NaN;
%                1500, NaN]
%  eqv = [ 8.2456, 87.1795, 891.8919;
%          0.9901, 11.0000,      NaN]
%
% % Circuit: four E24 components, sum values:
% [cmp,eqv] = equivcircuit([19; 10; 1982], 'E24', [1,1000], 4, false)
%  cmp = [ 1,   1,    2;
%          1,   1,  160;
%          1, 1.2,  820;
%         16, 6.8, 1000]
%  eqv = [19;  10; 1982]
%
% ### Output Indices ###
%
% For an input matrix X(x,y,..):
% - Equivalent circuit values matrix elements correspond: eqv(x,y,..).
% - Component value and the range index matrices' first dimension correspond
%   to each component, the other dimensions are permuted: cmp(c,x,y,..).
%   Thus each column always gives the components/indices for one input value.
%
% Example:
% [cmp,eqv,idx,vec] = equivcircuit(X,ser,rng,nmc,rec);
%
% ColFn = @(k) nmc*(k-1)+(1:nmc); % The indices of any column.
% for k = 1:numel(X) % For each input matrix element, get:
%     X(k)           % the input value.
%     eqv(k)         % the equivalent circuit value, and
%     cmp(ColFn(k))  % the required component values for that circuit.
%     idx(ColFn(k))  % the <vec> component location indices.
% end
%
% ### Input & Output Arguments ###
%
% Inputs:
%  X   = Numeric matrix, values to match using an equivalent circuit.
%  ser = StringToken, to select component E-series. See M-file "round60063".
%  rng = Numeric Vector, two elements: [Min,Max] permitted component values.
%  nmc = Numeric scalar, select the number of components in the circuit.
%  rec = Logical scalar, true/false -> reciprocal/normal sum of components.
%
% Outputs (all are numeric, input Inf & NaN values are returned as NaN):
%  cmp = Matrix, component values that sum to best match values of <X>.
%        The dimensions are based on <X> but are shifted by one, the first
%        dimension (columns) are thus the component values for each <X> value.
%  eqv = Matrix, equivalent circuit value using the components in <cmp>, each
%        element corresponds to <X>, dimensions are the same as <X>.
%  idx = Matrix, index showing which <vec> component values were
%        returned. The dimensions are the same as <cmp>.
%  vec = Vector, the component value series, contiguous values over <rng>.
%
% Inputs = (X,ser,rng,nmc,rec)
% Outputs = [cmp,eqv,idx,vec]

assert(isnumeric(X),'First input <X> must be a numeric scalar, vector or matrix.')
assert(isnumeric(rng)&&numel(rng)==2,'Third input <rng> must a two element numeric.')
assert(isnumeric(nmc)&&isscalar(nmc),'Fourth input <nmc> must be numeric scalar.')
assert(islogical(rec)&&isscalar(rec),'Fifth input <rec> must be a logical scalar.')
%
siz = size(X);
X = real(X(:));
% Location of finite values in matrices <X> and <cmp>:
fii = isfinite(X) & 0<X;
fio = reshape(repmat(fii.',nmc,1),[],1);
% Preallocate equivalent circuit, component value and PNS index matrices:
eqv = NaN(siz);
cmp = NaN([nmc,siz]);
idx = NaN([nmc,siz]);
%
% Get vector of PNS values:
[~,vec] = round60063(rng,ser);
%
if numel(X)<1||nmc<1
    return
end
%
% Column indices for inserting values into output matrices:
ColFn = @(n)nmc*(n-1)+(1:nmc);
%
PrfVec = ecRec(rec,vec(:));
PrfNmL = numel(PrfVec);
%
% Maximum permitted matrix elements (adjust to suit OS & computer):
MaxEl = pow2(22);
%
if MaxEl<PrfNmL^nmc% Method: iterate over all PNS multiset indices,
    % comparing the sum of each multiset. (Slow but no large matrices)
    %
    % Initial values:
    cpr = Inf(numel(X),1);% Previous best (aim = -Inf).
    idm = ones(nmc,1);% First multiset index.
    idx(fio) = 1;%  First multiset indices.
    eqv(fii) = ecRec(rec,sum(PrfVec(idm)));% Value at first index.
    %
    for p = 2:prod(PrfNmL:(PrfNmL+nmc-1))/prod(1:nmc)
        %
        % Determine the next unique multiset index:
        if idm(nmc)==PrfNmL
            cnt = nmc-1;
            while idm(cnt)==PrfNmL
                cnt = cnt-1;
            end
            idm(cnt:nmc) = 1+idm(cnt);
        else
            idm(nmc) = 1+idm(nmc);
        end
        %
        % Sum value to compare previous best with:
        vat = ecRec(rec,sum(PrfVec(idm)));
        cpt = log(abs(X-vat));
        % If the sum is closer, then replace previous best and multiset indices:
        q = cpt<cpr;
        cpr(q) = cpt(q);
        eqv(q) = vat;
        for k = find(q).'
            idx(ColFn(k)) = idm;
        end
        %
    end
    %
else% Method: Generate complete PNS-sum multiset matrix in one go, and
    % compare all possible sums at once. (Fast but uses large matrices)
    %
    SumSiz = PrfNmL*ones(1,nmc);
    SumMat = repmat(PrfVec,[1,SumSiz(2:end)]);% Large!
    %
    % Cumulative sums of all the PNS values, for all multisets:
    for m = 2:nmc
        PrfVec = shiftdim(PrfVec,-1);
        SumMat = bsxfun(@plus,SumMat,PrfVec);
    end
    SumMat = ecRec(rec,SumMat(:));
    %
    % Best matches of input values to equivalent circuits (sums):
    [~,idm] = min(log(abs(bsxfun(@minus,X.',SumMat))),[],1);
    %
    % Return equivalent values and component indices:
    for k = find(fii).'
        eqv(k) = SumMat(idm(k));
        idx(ColFn(k)) = ecInd2Sub(SumSiz,idm(k));
    end
    %
end
%
cmp(fio) = vec(idx(fio));
%
end
%----------------------------------------------------------------------END:equivcircuit
function val = ecRec(rec,val)
%
if rec
    val = 1./val;
end
%
end
%----------------------------------------------------------------------END:ecRec
function SubIdx = ecInd2Sub(MatSiz,LnIdx)
% Subscript index vector from matrix size and linear index.
%
SubIdx = MatSiz;
MatCum = [1,cumprod(MatSiz(1:end-1))];
for k = numel(MatSiz):-1:1
    RemIdx = 1+rem(LnIdx-1,MatCum(k));
    SubIdx(k) = 1+(LnIdx-RemIdx)/MatCum(k);
    LnIdx = RemIdx;
end
%
end
%----------------------------------------------------------------------END:ecInd2Sub