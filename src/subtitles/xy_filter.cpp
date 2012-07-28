#include "stdafx.h"

typedef const UINT8 CUINT8, *PCUINT8;

/****
 * See @xy_filter_c
 **/
__forceinline void xy_filter_one_line_c(float *dst, int width, const float *filter, int filter_width)
{
    const float *filter_start = filter;
    int xx_fix = width > filter_width ? 0 : filter_width - width;
    float *dst2 = dst - filter_width;
    float *dst_endr = dst + width;
    float *dst_end0 = dst_endr - filter_width;
    float *dst_endl = dst - xx_fix;
    ASSERT(xx_fix==0 || dst_end0==dst_endl);

    ASSERT(filter_start == filter);
    filter_start += filter_width;
    const float *filter_end = filter_start;
    for (;dst2<dst_endl;dst2++, filter_start--)//left margin
    {
        const float *src = dst;
        float sum = 0;
        for(const float* f=filter_start;f<filter_end;f++, src++)
        {
            sum += src[0] * f[0];
        }
        *dst2 = sum;
    }
    for (;dst2<dst;dst2++, filter_start--, filter_end--)//if width < filter_width
    {
        const float *src = dst;
        float sum = 0;
        for(const float* f=filter_start;f<filter_end;f++, src++)
        {
            sum += src[0] * f[0];
        }
        *dst2 = sum;
    }
    ASSERT(filter_start==filter);
    for (;dst2<dst_end0;dst2++)
    {
        const float *src = dst2;

        float sum = 0;
        for(const float* f=filter_start;f<filter_end;f++, src++)
        {
            sum += src[0] * f[0];
        }
        *dst2 = sum;
    }
    for (;dst2<dst_endr;dst2++, filter_end--)//right margin
    {
        const float *src = dst2;
        float sum = 0;
        for(const float* f=filter;f<filter_end;f++, src++)
        {
            sum += src[0] * f[0];
        }
        *dst2 = sum;
    }
}

/****
 * dst memory layout:
 * 1. Source content starts from @dst;
 * 2. Output content starts from @dst-@mwitdth;
 * 
 * |-                     stride                            -|
 * | <- @dst-@mwidth --------| <- @dst+0 --------------------|
 * |-                       -|-                             -|
 * |- margin                -|-  src width*height items     -|
 * |- mwidth*heigth items   -|-                             -|
 * |- do NOT need to init   -|-                             -|
 * |- when input            -|-                             -|
 * |---------------------------------------------------------|
 **/
void xy_filter_c(float *dst, int width, int height, int stride, const float *filter, int filter_width)
{
    ASSERT( stride>=4*(width+filter_width) );
    BYTE* end = reinterpret_cast<BYTE*>(dst) + height*stride;
    BYTE* dst_byte = reinterpret_cast<BYTE*>(dst);
    for( ; dst_byte<end; dst_byte+=stride )
    {
        xy_filter_one_line_c(reinterpret_cast<float*>(dst_byte), width, filter, filter_width);
    }
}

/****
 * inline function sometimes generates stupid code
 * 
 * @src4, @src_5_8, @f4, @sum : __m128
 * @src_5_8, @f4: const
 * @sum : output
 * @src4: undefined
 **/ 
#define XY_FILTER_4(src4, src_5_8, f4, sum) \
    __m128 f4_1 = _mm_shuffle_ps(f4, f4, _MM_SHUFFLE(0,0,0,0));\
    f4_1 = _mm_mul_ps(f4_1, src4);\
    sum = _mm_add_ps(sum, f4_1);\
    __m128 src_3_6 = _mm_shuffle_ps(src4, src_5_8, _MM_SHUFFLE(1,0,3,2));/*3 4 5 6*/\
    f4_1 = _mm_shuffle_ps(f4, f4, _MM_SHUFFLE(2,2,2,2));\
    f4_1 = _mm_mul_ps(f4_1, src_3_6);\
    sum = _mm_add_ps(sum, f4_1);\
    src4 = _mm_shuffle_ps(src4, src_3_6, _MM_SHUFFLE(2,1,2,1));/*2 3 4 5*/\
    f4_1 = _mm_shuffle_ps(f4, f4, _MM_SHUFFLE(1,1,1,1));\
    f4_1 = _mm_mul_ps(f4_1, src4);\
    sum = _mm_add_ps(sum, f4_1);\
    src_3_6 = _mm_shuffle_ps(src_3_6, src_5_8, _MM_SHUFFLE(2,1,2,1));/*4 5 6 7*/\
    f4_1 = _mm_shuffle_ps(f4, f4, _MM_SHUFFLE(3,3,3,3));\
    f4_1 = _mm_mul_ps(f4_1, src_3_6);\
    sum = _mm_add_ps(sum, f4_1)


__forceinline void xy_filter_one_line_sse_v4(float *dst, int width, const float *filter, int filter_width)
{
    int xx_fix = width > filter_width ? 0 : filter_width - width;
    const float *filter_start = filter;
    float *dst2 = dst - filter_width;
    float *dst_endr = dst + width;
    float *dst_end0 = dst_endr - filter_width;
    float *dst_endl = dst - xx_fix;
    ASSERT(xx_fix==0 || dst_end0==dst_endl);

    ASSERT(filter_start == filter);
    filter_start += filter_width;
    const float *filter_end = filter_start;

    for (;dst2<dst_endl;dst2+=4)//left margin
    {
        const float *src = dst;
        filter_start -= 4;

        //filter 4
        __m128 src4 = _mm_setzero_ps();/*1 2 3 4*/
        __m128 sum = _mm_setzero_ps();
        for(const float* f=filter_start;f<filter_end;f+=4,src+=4)
        {   
            __m128 src_5_8 = _mm_load_ps(src);/*5 6 7 8*/
            __m128 f4 = _mm_load_ps(f);

            { XY_FILTER_4(src4, src_5_8, f4, sum); }

            src4 = src_5_8;
        }
        //store result
        _mm_store_ps(dst2, sum);
    }
    for (;dst2<dst;dst2+=4)//if width < filter_width
    {
        const float *src = dst;
        filter_start-=4;
        filter_end-=4;

        __m128 src4 = _mm_setzero_ps();/*1 2 3 4*/
        __m128 sum = _mm_setzero_ps();
        __m128 src_5_8, f4;
        for(const float* f=filter_start;f<filter_end;f+=4,src+=4)
        {   
            src_5_8 = _mm_load_ps(src);/*5 6 7 8*/
            f4 = _mm_load_ps(f);

            { XY_FILTER_4(src4, src_5_8, f4, sum); }
            src4 = src_5_8;
        }
        src_5_8 = _mm_setzero_ps();
        f4 = _mm_load_ps(filter_end);
        { XY_FILTER_4(src4, src_5_8, f4, sum); }
        //store result
        _mm_store_ps(dst2, sum);
    }
    ASSERT(filter_start == filter);
    for (;dst2<dst_end0;dst2+=4)
    {
        const float *src = dst2;

        //filter 4
        __m128 src4 = _mm_load_ps(src);/*1 2 3 4*/
        __m128 sum = _mm_setzero_ps();
        for(const float* f=filter_start;f<filter_end;f+=4)
        {
            src+=4;
            __m128 src_5_8 = _mm_load_ps(src);/*5 6 7 8*/
            __m128 f4 = _mm_load_ps(f);

            { XY_FILTER_4(src4, src_5_8, f4, sum); }
            src4 = src_5_8;
        }
        //store result
        _mm_store_ps(dst2, sum);
    }
    for (;dst2<dst_endr;dst2+=4)//right margin
    {
        const float *src = dst2;
        filter_end-=4;

        //filter 4
        __m128 src4 = _mm_load_ps(src);//1 2 3 4
        __m128 sum = _mm_setzero_ps();
        __m128 src_5_8, f4;
        for(const float* f=filter_start;f<filter_end;f+=4)
        {
            src+=4;
            src_5_8 = _mm_load_ps(src);//5 6 7 8
            f4 = _mm_load_ps(f);

            { XY_FILTER_4(src4, src_5_8, f4, sum); }

            src4 = src_5_8;
            //move new 4 in_n_out to old 4 in_n_out
        }
        src_5_8 = _mm_setzero_ps();
        f4 = _mm_load_ps(filter_end);
        { XY_FILTER_4(src4, src_5_8, f4, sum); }
        //store result
        _mm_store_ps(dst2, sum);
    }
}

/****
 * See @xy_filter_c
 **/
void xy_filter_sse_v4(float *dst, int width, int height, int stride, const float *filter, int filter_width)
{
    ASSERT( stride>=4*(width+filter_width) );
    ASSERT( ((stride|(4*width)|(4*filter_width)|reinterpret_cast<int>(dst)|reinterpret_cast<int>(filter))&15)==0 );

    BYTE* dst_byte = reinterpret_cast<BYTE*>(dst);
    BYTE* end = dst_byte + height*stride;
    for( ; dst_byte<end; dst_byte+=stride )
    {
        xy_filter_one_line_sse_v4(reinterpret_cast<float*>(dst_byte), width, filter, filter_width);
    }
}

template<int LENGTH>
struct M128s
{
    __m128 x;
    M128s<LENGTH - 4> next;

    template<int Index> __forceinline __m128& GetAt()
    {
        return next.GetAt<Index - 4>();
    }
    template<> __forceinline __m128& GetAt<0>()
    {
        return x;
    }

    template<int Start, int Offset> __forceinline __m128& GetAt()
    {
        return GetAt<Start + Offset>();
    }

    __forceinline void Load(const float* src)
    {
        x = _mm_load_ps(src);
        next.Load(src+4);
    }
};

template<>
struct M128s<0>
{
    void Load(const float* src)
    {
    }
};

template<int FILTER_LENGTH, int START, int LENGTH>
struct Filter4
{
    static __forceinline void do_cal(__m128& src0_128, const float * src4, M128s<FILTER_LENGTH>& filter128s, __m128& sum)
    {
        Filter4<FILTER_LENGTH,START,LENGTH-4>::do_cal(src0_128, src4, filter128s, sum);
        __m128 src4_128 = _mm_load_ps(src4+LENGTH-4);
        XY_FILTER_4(src0_128, src4_128, filter128s.GetAt<START+LENGTH-4>(), sum);
        src0_128 = src4_128;
    }
};

template<int FILTER_LENGTH, int START>
struct Filter4<FILTER_LENGTH, START, 0>
{
    static __forceinline void do_cal(__m128& src0_128, const float * src4, M128s<FILTER_LENGTH>& filter128s, __m128& sum)
    {
    }
};

template<int FILTER_LENGTH,int MARGIN_LENGTH>
struct FilterAllMargin
{
    static __forceinline void cal_left_margin(float * src, M128s<FILTER_LENGTH>& filter128s)
    {
        //filter 4
        __m128 src0 = _mm_setzero_ps();
        __m128 sum = _mm_setzero_ps();
        Filter4<FILTER_LENGTH,MARGIN_LENGTH-4,FILTER_LENGTH-MARGIN_LENGTH+4>::do_cal(src0, src, filter128s, sum);
        _mm_store_ps(src-MARGIN_LENGTH, sum);
        FilterAllMargin<FILTER_LENGTH,MARGIN_LENGTH-4>::cal_left_margin(src, filter128s);
    }

    static __forceinline void cal_right_margin(float * src, M128s<FILTER_LENGTH>& filter128s)
    {
        //filter 4
        {
            __m128 src0 = _mm_load_ps(src);
            __m128 sum = _mm_setzero_ps();
            Filter4<FILTER_LENGTH,0,MARGIN_LENGTH-4>::do_cal(src0, src+4, filter128s, sum);
            __m128 src4 = _mm_setzero_ps();
            XY_FILTER_4(src0, src4, filter128s.GetAt<MARGIN_LENGTH-4>(), sum);
            //store result
            _mm_store_ps(src, sum);
        }
        FilterAllMargin<FILTER_LENGTH,MARGIN_LENGTH-4>::cal_right_margin(src+4, filter128s);
    }
};

template<int FILTER_LENGTH>
struct FilterAllMargin<FILTER_LENGTH,0>
{
    static __forceinline void cal_left_margin(float * src, M128s<FILTER_LENGTH>& filter128s)
    {
    }
    static __forceinline void cal_right_margin(float * src, M128s<FILTER_LENGTH>& filter128s)
    {
    }
};

/****
 * See @xy_filter_c
 * Constrain:
 *   FILTER_LENGTH%4 == 0 && FILTER_LENGTH<=width
 **/
template<int FILTER_LENGTH>
void xy_filter_sse_template(float *dst, int width, int height, int stride, const float *filter)
{
    ASSERT( stride>=4*(width+FILTER_LENGTH) );
    ASSERT( ((stride|(4*width)|(4*FILTER_LENGTH)|reinterpret_cast<int>(dst)|reinterpret_cast<int>(filter))&15)==0 );

    M128s<FILTER_LENGTH> filter128s;
    filter128s.Load(filter);

    const float *filter_start = filter;
    BYTE* dst_byte = reinterpret_cast<BYTE*>(dst);
    BYTE* end = dst_byte + height*stride;
    for( ; dst_byte<end; dst_byte+=stride )
    {
        float *dst2 = reinterpret_cast<float*>(dst_byte);

        //left margin
        FilterAllMargin<FILTER_LENGTH,FILTER_LENGTH>::cal_left_margin(dst2, filter128s);
        float *dst_end1 = dst2 + width;
        float *dst_end0 = dst_end1 - FILTER_LENGTH;
        for (;dst2<dst_end0;dst2+=4)
        {
            const float *src = dst2;

            //filter 4
            __m128 src0 = _mm_load_ps(src);/*1 2 3 4*/
            src += 4;
            __m128 sum = _mm_setzero_ps();
            Filter4<FILTER_LENGTH,0,FILTER_LENGTH>::do_cal(src0, src, filter128s, sum);
            //store result
            _mm_store_ps(dst2, sum);
        }
        FilterAllMargin<FILTER_LENGTH,FILTER_LENGTH>::cal_right_margin(dst2, filter128s);
    }
}

/****
 * See @xy_filter_c
 **/
void xy_filter_sse(float *dst, int width, int height, int stride, const float *filter, int filter_width)
{
    typedef void (*Filter)(float *dst, int width, int height, int stride, const float *filter);
    const Filter filters[] = { xy_filter_sse_template<4>, xy_filter_sse_template<8>, xy_filter_sse_template<12>, xy_filter_sse_template<16>,
        xy_filter_sse_template<20>, xy_filter_sse_template<24>, xy_filter_sse_template<28>
    };
    if (filter_width<=28 && filter_width<=width)
    {
        ASSERT(filter_width%4==0);
        filters[(filter_width-1)/4](dst, width, height, stride, filter);
    }
    else
    {
        xy_filter_sse_v4(dst, width, height, stride, filter, filter_width);
    }   
}

/****
 * Copy and convert src to dst line by line.
 * @dst_width MUST >= @width
 * if @dst_width>@width, the extra elements will be filled with 0.
 **/
void xy_byte_2_float_c(float *dst, int dst_width, int dst_stride, 
    PCUINT8 src, int width, int height, int stride)
{
    PUINT8 dst_byte = reinterpret_cast<PUINT8>(dst);

    PCUINT8 src_end = src + height*stride;
    for( ; src<src_end; src+=stride, dst_byte+=dst_stride )
    {
        PCUINT8 src2 = src;
        PCUINT8 src_end = src2 + width;
        float *dst2 = reinterpret_cast<float*>(dst_byte);

        for (;src2<src_end;src2++, dst2++)
        {
            *dst2 = *src2;
        }
        float *dst2_end=reinterpret_cast<float*>(dst_byte)+dst_width;
        for (;dst2<dst2_end;dst2++)
        {
            *dst2=0;
        }
    }
}

/****
 * See @xy_byte_2_float_c
 **/
void xy_byte_2_float_sse(float *dst, int dst_width, int dst_stride, 
    PCUINT8 src, int width, int height, int stride)
{
    ASSERT( dst_width>=width );
    ASSERT( ((reinterpret_cast<int>(dst)|dst_stride)&15)==0 );
    PUINT8 dst_byte = reinterpret_cast<PUINT8>(dst);

    PCUINT8 src_end = src + height*stride;
    for( ; src<src_end; src+=stride, dst_byte+=dst_stride )
    {
        PCUINT8 src2 = src;
        PCUINT8 src2_end0 = src2 + (width&~15); 
        float *dst2 = reinterpret_cast<float*>(dst_byte);

        for (;src2<src2_end0;src2+=16, dst2+=16)
        {
            //filter 4
            __m128i src16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src2));
            __m128i src16_lo = _mm_unpacklo_epi8(src16, _mm_setzero_si128()); 
            __m128i src16_hi = _mm_unpackhi_epi8(src16, _mm_setzero_si128());
            __m128i src16_lo_lo = _mm_unpacklo_epi8(src16_lo, _mm_setzero_si128());
            __m128i src16_lo_hi = _mm_unpackhi_epi8(src16_lo, _mm_setzero_si128());
            __m128i src16_hi_lo = _mm_unpacklo_epi8(src16_hi, _mm_setzero_si128());
            __m128i src16_hi_hi = _mm_unpackhi_epi8(src16_hi, _mm_setzero_si128());
            __m128 dst_f1 =  _mm_cvtepi32_ps(src16_lo_lo);
            __m128 dst_f2 =  _mm_cvtepi32_ps(src16_lo_hi);
            __m128 dst_f3 =  _mm_cvtepi32_ps(src16_hi_lo);
            __m128 dst_f4 =  _mm_cvtepi32_ps(src16_hi_hi);
            _mm_store_ps(dst2, dst_f1);
            _mm_store_ps(dst2+4, dst_f2);
            _mm_store_ps(dst2+8, dst_f3);
            _mm_store_ps(dst2+12, dst_f4);
        }
        PCUINT8 src2_end = src + width;
        for (;src2<src2_end;src2++,dst2++)
        {
            *dst2 = *src2;
        }
        float *dst2_end=reinterpret_cast<float*>(dst_byte)+dst_width;
        for (;dst2<dst2_end;dst2++)
        {
            *dst2=0;
        }
    }
}

/****
 * Copy transposed Matrix src to dst.
 * @dst_width MUST >= @height.
 * if @dst_width > @height, the extra elements will be filled with 0.
 **/
void xy_float_2_float_transpose_c(float *dst, int dst_width, int dst_stride, 
    const float *src, int width, int height, int src_stride)
{
    ASSERT(dst_width >= height);
    PUINT8 dst_byte = reinterpret_cast<PUINT8>(dst);
    const float* src_end = src + width;
    PCUINT8 src2_end = reinterpret_cast<PCUINT8>(src) + height*src_stride;
    for( ; src<src_end; src++, dst_byte+=dst_stride )
    {
        PCUINT8 src2 = reinterpret_cast<PCUINT8>(src);

        float *dst2 = reinterpret_cast<float*>(dst_byte);        
        for (;src2<src2_end;src2+=src_stride,dst2++)
        {
            *dst2 = *reinterpret_cast<const float*>(src2);
        }
        float *dst2_end = reinterpret_cast<float*>(dst_byte) + dst_width;
        for (;dst2<dst2_end;dst2++)
        {
            *dst2 = 0;
        }
    }
}

/****
 * see @xy_float_2_float_transpose_c
 **/
void xy_float_2_float_transpose_sse(float *dst, int dst_width, int dst_stride, 
    const float *src, int width, int height, int src_stride)
{
    typedef float DstT;
    typedef const float SrcT;

    ASSERT( (((int)dst|dst_stride)&15)==0 );
    ASSERT(dst_width >= height);
    PUINT8 dst_byte = reinterpret_cast<PUINT8>(dst);
    SrcT* src_end = src + width;
    PCUINT8 src2_end1 = reinterpret_cast<PCUINT8>(src) + (height&~3)*src_stride;
    PCUINT8 src2_end2 = reinterpret_cast<PCUINT8>(src) + height*src_stride;
    for( ; src<src_end; src++, dst_byte+=dst_stride )
    {
        PCUINT8 src2 = reinterpret_cast<PCUINT8>(src);

        DstT *dst2 = reinterpret_cast<DstT*>(dst_byte);
        for (;src2<src2_end1;src2+=4*src_stride,dst2+=4)
        {
            __m128 m1 = _mm_set_ps(
                *(SrcT*)(src2+3*src_stride), 
                *(SrcT*)(src2+2*src_stride), 
                *(SrcT*)(src2+src_stride), 
                *(SrcT*)(src2));
            _mm_store_ps(dst2, m1);
        }
        for (;src2<src2_end2;src2+=src_stride,dst2++)
        {
            *dst2 = *reinterpret_cast<SrcT*>(src2);
        }
        float *dst2_end = reinterpret_cast<DstT*>(dst_byte) + dst_width;
        for (;dst2<dst2_end;dst2++)
        {
            *dst2 = 0;
        }
    }
}

/****
 * Transpose and round Matrix src, then copy to dst.
 * @dst_width MUST >= @height.
 * if @dst_width > @height, the extra elements will be filled with 0.
 **/
void xy_float_2_byte_transpose_c(UINT8 *dst, int dst_width, int dst_stride, 
    const float *src, int width, int height, int src_stride)
{
    ASSERT(dst_width >= height);
    PUINT8 dst_byte = reinterpret_cast<PUINT8>(dst);
    const float* src_end = src + width;
    PCUINT8 src2_end = reinterpret_cast<PCUINT8>(src) + height*src_stride;
    for( ; src<src_end; src++, dst_byte+=dst_stride )
    {
        PCUINT8 src2 = reinterpret_cast<PCUINT8>(src);

        UINT8 *dst2 = reinterpret_cast<UINT8*>(dst_byte);
        for (;src2<src2_end;src2+=src_stride,dst2++)
        {
            *dst2 = static_cast<UINT8>(*reinterpret_cast<const float*>(src2)+0.5);
        }
        UINT8 *dst2_end = reinterpret_cast<UINT8*>(dst_byte) + dst_width;
        for (;dst2<dst2_end;dst2++)
        {
            *dst2 = 0;
        }
    }
}

void xy_float_2_byte_transpose_sse(UINT8 *dst, int dst_width, int dst_stride, 
    const float *src, int width, int height, int src_stride)
{
    typedef UINT8 DstT;
    typedef const float SrcT;

    ASSERT(dst_width >= height);
    ASSERT((((int)dst|dst_stride)&15)==0);
    PUINT8 dst_byte = reinterpret_cast<PUINT8>(dst);
    SrcT* src_end = src + width;
    PCUINT8 src2_end00 = reinterpret_cast<PCUINT8>(src) + (height&~15)*src_stride;
    PCUINT8 src2_end = reinterpret_cast<PCUINT8>(src) + height*src_stride;
    __m128 m0 = _mm_set1_ps(0.5f);
    for( ; src<src_end; src++, dst_byte+=dst_stride )
    {
        PCUINT8 src2 = reinterpret_cast<PCUINT8>(src);

        DstT *dst2 = reinterpret_cast<DstT*>(dst_byte);
        for (;src2<src2_end00;src2+=16*src_stride,dst2+=16)
        {
            __m128 m1 = _mm_set_ps(
                *(SrcT*)(src2+3*src_stride),
                *(SrcT*)(src2+2*src_stride), 
                *(SrcT*)(src2+src_stride), 
                *(SrcT*)(src2));
            __m128 m2 = _mm_set_ps(
                *(SrcT*)(src2+7*src_stride),
                *(SrcT*)(src2+6*src_stride),
                *(SrcT*)(src2+5*src_stride),
                *(SrcT*)(src2+4*src_stride));
            __m128 m3 = _mm_set_ps(
                *(SrcT*)(src2+11*src_stride),
                *(SrcT*)(src2+10*src_stride),
                *(SrcT*)(src2+9*src_stride), 
                *(SrcT*)(src2+8*src_stride));
            __m128 m4 = _mm_set_ps(
                *(SrcT*)(src2+15*src_stride),
                *(SrcT*)(src2+14*src_stride),
                *(SrcT*)(src2+13*src_stride),
                *(SrcT*)(src2+12*src_stride));
            m1 = _mm_add_ps(m1,m0);
            m2 = _mm_add_ps(m2,m0);
            m3 = _mm_add_ps(m3,m0);
            m4 = _mm_add_ps(m4,m0);

            __m128i i1 = _mm_cvtps_epi32(m1);
            __m128i i2 = _mm_cvtps_epi32(m2);
            __m128i i3 = _mm_cvtps_epi32(m3);
            __m128i i4 = _mm_cvtps_epi32(m4);

            i1 = _mm_packs_epi32(i1,i2);
            i3 = _mm_packs_epi32(i3,i4);
            i1 = _mm_packus_epi16(i1,i3);

            _mm_store_si128((__m128i*)dst2, i1);
        }
        for (;src2<src2_end;src2+=src_stride,dst2++)
        {
            *dst2 = static_cast<DstT>(*reinterpret_cast<SrcT*>(src2)+0.5);
        }
        DstT *dst2_end = reinterpret_cast<DstT*>(dst_byte) + dst_width;
        for (;dst2<dst2_end;dst2++)
        {
            *dst2 = 0;
        }
    }
}

/****
 * To Do: decent CPU capability check
 **/
void xy_gaussian_blur(PUINT8 dst, int dst_stride,
    PCUINT8 src, int width, int height, int stride, 
    const float *gt, int r, int gt_ex_width)
{
    ASSERT(width<=stride && width+2*r<=dst_stride);
    int ex_mask_width = ((r*2+1)+3)&~3;
    ASSERT(ex_mask_width<=gt_ex_width);
    if (ex_mask_width>gt_ex_width)
    {
        int o=0;
        o=o/o;
        exit(-1);
    }

    int fwidth = (width+3)&~3;
    int fstride = (fwidth + ex_mask_width)*sizeof(float);
    int fheight = (height+3)&~3;
    int fstride_ver = (fheight+ex_mask_width)*sizeof(float);

    PUINT8 buff_base = reinterpret_cast<PUINT8>(xy_malloc(height*fstride + (fwidth + ex_mask_width)*fstride_ver));
    
    float *hor_buff_base = reinterpret_cast<float*>(buff_base);
    float *hor_buff = hor_buff_base + ex_mask_width;    

    // byte to float
    ASSERT( ((width+15)&~15)<=stride );
    xy_byte_2_float_sse(hor_buff, fwidth, fstride, src, width, height, stride);

    // horizontal pass
    xy_filter_sse(hor_buff, fwidth, height, fstride, gt, ex_mask_width);


    // transpose
    float *ver_buff_base = reinterpret_cast<float*>(buff_base + height*fstride);
    float *ver_buff = ver_buff_base + ex_mask_width;

    int true_width = width+r*2;
    xy_float_2_float_transpose_sse(ver_buff, fheight, fstride_ver, hor_buff-r*2, true_width, height, fstride);

    // vertical pass
    xy_filter_sse(ver_buff, fheight, true_width, fstride_ver, gt, ex_mask_width);
    
    // transpose
    int true_height = height + 2*r;
    xy_float_2_byte_transpose_sse(dst, true_width, dst_stride, ver_buff-r*2, true_height, true_width, fstride_ver);
    
    xy_free(buff_base);
    _mm_empty();
}