package com.springboot.service;

import com.springboot.entities.Guangzhao;
import com.springboot.entities.Wendu;
import com.springboot.repositroy.GuangzhaoRepositoty;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import javax.transaction.Transactional;
import java.util.Date;
import java.util.List;

/**
 * Created by 10497 on 2017/6/12.
 */
@Service
@Transactional
public class GuangzhaoService {
    @Autowired
    private GuangzhaoRepositoty guangzhaoRepositoty;
    public List<Guangzhao> findAllGuangzhao(){
        List<Guangzhao> list = guangzhaoRepositoty.findAllGuangzhao();
        return  list;
    }
    public  void addGuangzhao(int gz){
        Guangzhao guangzhao=new Guangzhao();
        guangzhao.setGz(gz);
        guangzhao.setTime(new Date());
        guangzhaoRepositoty.saveAndFlush(guangzhao);
    }
}
